// Emisar D3AA helper functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fsm/chan-rgbaux.c"
#include "fsm/ramping.h"
#include "ui/anduril/misc.h"

void set_level_zero();

void set_level_main(uint8_t level);
bool gradual_tick_main(uint8_t gt);


Channel channels[] = {
    { // main LEDs
        .set_level    = set_level_main,
        .gradual_tick = gradual_tick_main
    },
    RGB_AUX_CHANNELS
};


void set_level_zero() {
    DAC_LVL  = 0;  // DAC off
    DAC_VREF = V10;  // low Vref
    HDR_ENABLE_PORT &= ~(1 << HDR_ENABLE_PIN);  // HDR off

    // prevent post-off flash
    IN_NFET_ENABLE_PORT |= (1 << IN_NFET_ENABLE_PIN);
    delay_4ms(IN_NFET_DELAY_TIME/4);
    IN_NFET_ENABLE_PORT &= ~(1 << IN_NFET_ENABLE_PIN);

    // turn off boost last
    BST_ENABLE_PORT &= ~(1 << BST_ENABLE_PIN);  // BST off
}

// single set of LEDs with 1 regulated power channel
// and low/high HDR plus low/high Vref as different "gears"
void set_level_main(uint8_t level) {
    uint8_t noflash = 0;

    // when turning on from off, use IN_NFET to prevent a flash
    if ((! actual_level) && (level < HDR_ENABLE_LEVEL_MIN)) {
        noflash = 1;
        IN_NFET_ENABLE_PORT |= (1 << IN_NFET_ENABLE_PIN);
    }

    // BST on first, to give it a few extra microseconds to spin up
    BST_ENABLE_PORT |= (1 << BST_ENABLE_PIN);

    // pre-load ramp data so it can be assigned faster later
    // DAC level register is left-aligned
    PWM1_DATATYPE dac_lvl  = PWM1_GET(level) << 6;
    PWM2_DATATYPE dac_vref = PWM2_GET(level);

    // enable HDR on top half of ramp
    if (level >= (HDR_ENABLE_LEVEL_MIN-1))
        HDR_ENABLE_PORT |= (1 << HDR_ENABLE_PIN);
    else
        HDR_ENABLE_PORT &= ~(1 << HDR_ENABLE_PIN);

    // set these in successive clock cycles to avoid getting out of sync
    // (minimizes ramp bumps when changing gears)
    DAC_LVL  = dac_lvl;
    DAC_VREF = dac_vref;

    if (noflash) {
        // wait for flash prevention to finish
        delay_4ms(IN_NFET_DELAY_TIME/4);
        IN_NFET_ENABLE_PORT &= ~(1 << IN_NFET_ENABLE_PIN);
    }
}

bool gradual_tick_main(uint8_t gt) {
    // if HDR and Vref "engine gear" is the same, do a small adjustment...
    // otherwise, simply jump to the next ramp level
    //   and let set_level() handle any gear changes

    // different gear = full adjustment
    PWM2_DATATYPE vref_next = PWM2_GET(gt);
    if (vref_next != DAC_VREF) return true;  // let parent set_level() for us

    // same gear = small adjustment
    PWM1_DATATYPE dac_now  = DAC_LVL >> 6;  // register is left-aligned
    PWM1_DATATYPE dac_next = PWM1_GET(gt);

    // only adjust 1 dac level, max is 1023
    // (but speed it up with "#define GRADUAL_ADJUST_SPEED  4" elsewhere)
    GRADUAL_ADJUST_SIMPLE(dac_next, dac_now);

    DAC_LVL = dac_now << 6;

    if (dac_next == dac_now) return true;  // done

    return false;  // not done yet
}


#ifdef USE_VOLTAGE_DIVIDER
uint8_t voltage_raw2cooked(uint16_t measurement) {
    // In : 65535 * BATTLVL / 1.024V
    // Out: uint8_t: Vbat * 50
    // BATTLVL = Vbat * (100.0/(330+100)) = Vbat / 4.3
    // So, Out = In * 4.3 / 1280
    uint8_t result = (uint32_t)(measurement + (65535 * 4 / 1024))
                     * 43 / 12800;
    return result;
}
#endif

#ifdef USE_WEAK_BATTERY_PROTECTION
uint8_t quick_volt_measurement() {
    // wait for next hardware measurement
    irq_adc = 0;
    while (! irq_adc) {}
    uint16_t m = adc_raw[0];
    return voltage_raw2cooked(m);
}

void detect_weak_battery() {
    // guess at the cell strength with a load test...
    // - measure voltage with LEDs off
    // - measure again with LEDs on
    // - determine how much to limit power
    //   (ramp up until battery becomes unstable)
    // - blink to indicate weak battery mode, if active

    ramp_level_hard_limit = 0;

    uint16_t resting, loaded;

    // baseline unloaded measurement
    set_level(0);
    for (uint8_t i=0; i<32; i++) { delay_zero(); }  // wait about 10ms
    //resting = voltage_raw2cooked(adc_smooth[0]);  // probably not settled yet
    resting = quick_volt_measurement();

    // set thresholds per cell type
    uint8_t sag_limit, crit_voltage;
    if (resting > DUAL_VOLTAGE_FLOOR) {
        sag_limit    = WEAK_BATTERY_SAG_THRESHOLD_LIION;
        crit_voltage = VOLTAGE_LOW;
    } else {
        sag_limit    = WEAK_BATTERY_SAG_THRESHOLD_AA;
        crit_voltage = DUAL_VOLTAGE_LOW_LOW;
    }

    // progressively turn up the power until sag threshold is hit,
    // or critical voltage, or max testing level is reached
    for (uint8_t l=1; l<WEAK_BATTERY_TEST_MAX_LEVEL; l++) {
        set_level(l);
        loaded = quick_volt_measurement();
        int16_t sag = resting - loaded;
        if ( (loaded <= crit_voltage) || (sag > sag_limit) ) {
            // battery empty or weak
            ramp_level_hard_limit = l;
            break;
        }
    }
    set_level(0);

    // Blink again if not in full-power mode:
    // - 1 blink total: Strong Li-ion cell, full power enabled
    // - 2 blinks: Strong AA cell, max AA power enabled
    //   (not used on this driver, strong AA uses mode 1)
    // - 3 blinks: Weak battery, power severely limited

    uint8_t extra_blinks = 0;
    if (ramp_level_hard_limit) extra_blinks += 2;

    for (uint8_t i=0; i<extra_blinks; i++) {
        delay_4ms(300/4);
        blink_once();
    }

    #ifdef USE_WEAK_BATTERY_PROTECTION_READOUT
    // this numeric display isn't really needed by default,
    // but the code remains in case anyone wants to use it
    if (ramp_level_hard_limit) {
        delay_4ms(255);
        // not booted far enough for this to work yet
        //blink_num(ramp_level_hard_limit);
        uint8_t tens, ones;
        tens = ramp_level_hard_limit / 10;
        ones = ramp_level_hard_limit % 10;
        for (uint8_t i=0; i<tens; i++) {
            delay_4ms(300/4);
            blink_once();
        }
        delay_4ms(600/4);
        for (uint8_t i=0; i<ones; i++) {
            delay_4ms(300/4);
            blink_once();
        }
    }
    #endif  // ifdef USE_WEAK_BATTERY_PROTECTION_READOUT

}
#endif  // ifdef USE_WEAK_BATTERY_PROTECTION

