// Emisar D3AA helper functions
// Copyright (C) 2023-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "anduril/channel-modes.h"  // for circular_tint_3h()
#include "fsm/chan-rgbaux.c"
#include "fsm/ramping.h"
#include "ui/anduril/misc.h"  // for blink_once()

void set_level_zero();

void set_level_main(uint8_t level);
bool gradual_tick_main(uint8_t gt);

void enable_auxrgb_pwm();
void disable_auxrgb_pwm();
rgb_uint_t get_level_auxrgb(uint8_t level);
void set_auxrgb_pwm(RGB8_t color);
void set_level_hsv(uint8_t level);
bool gradual_tick_hsv(uint8_t gt);

Channel channels[] = {
    { // main LEDs
        .set_level    = set_level_main,
        .gradual_tick = gradual_tick_main,
        .flags        = 0
    },
    { // aux RGB LEDs
        .set_level    = set_level_hsv,
        .gradual_tick = gradual_tick_hsv,
        .flags        = CHANNEL_FLAG_IS_AUX | CHANNEL_FLAG_HAS_ARGS
    },
    AUXRGB_CHANNELS
};

// HSV mode needs a different 3H handler
StatePtr channel_3H_modes[NUM_CHANNEL_MODES] = {
    NULL, circular_tint_3h,
};

inline void nfet_delay() {
    #if IN_NFET_DELAY_TIME > 0
    delay_4ms(IN_NFET_DELAY_TIME/4);
    #else
    delay_zero();
    delay_zero();
    #endif
}

void set_level_zero() {
    DAC_LVL  = 0;  // DAC off
    DAC_VREF = V10;  // low Vref
    HDR_ENABLE_PORT &= ~(1 << HDR_ENABLE_PIN);  // HDR off

    if (actual_level) {
        // prevent post-off flash
        IN_NFET_ENABLE_PORT |= (1 << IN_NFET_ENABLE_PIN);
        nfet_delay();
        IN_NFET_ENABLE_PORT &= ~(1 << IN_NFET_ENABLE_PIN);
    }

    // turn off boost last
    BST_ENABLE_PORT &= ~(1 << BST_ENABLE_PIN);  // BST off

    // turn off PWM for aux RGB
    disable_auxrgb_pwm();
}

// single set of LEDs with 1 regulated power channel
// and low/high HDR plus low/high Vref as different "gears"
void set_level_main(uint8_t level) {
    uint8_t noflash = 0;

    // when turning on from off, use IN_NFET to prevent a flash
    if ((! actual_level) && (level < HDR_ENABLE_LEVEL_MIN)) {
        noflash = 1;
        IN_NFET_ENABLE_PORT |= (1 << IN_NFET_ENABLE_PIN);
        //nfet_delay();
    }

    // pre-load ramp data so it can be assigned faster later
    // DAC level register is left-aligned
    PWM1_DATATYPE dac_lvl  = PWM1_GET(level) << 6;
    PWM2_DATATYPE dac_vref = PWM2_GET(level);

    // set these in successive clock cycles to avoid getting out of sync
    // (minimizes ramp bumps when changing gears)
    DAC_LVL  = dac_lvl;
    DAC_VREF = dac_vref;

    // enable HDR on top half of ramp
    if (level >= (HDR_ENABLE_LEVEL_MIN-1))
        HDR_ENABLE_PORT |= (1 << HDR_ENABLE_PIN);
    else
        HDR_ENABLE_PORT &= ~(1 << HDR_ENABLE_PIN);

    // if turning on from off, let things stabilize before enabling power
    if (noflash) { nfet_delay(); }

    // BST on last, after its inputs are set and stabilized
    BST_ENABLE_PORT |= (1 << BST_ENABLE_PIN);

    if (noflash) {
        // wait for flash prevention to finish
        nfet_delay();
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

///// RGB aux PWM stuff

void enable_auxrgb_pwm() {
    // set up the PWM for aux RGB
    // AVR32_16DD20_14_Prel_DataSheet_DS40002413-2997818.pdf
    // data sheet section 23.6 Register Summary - Split Mode
    // PA0 is TCA0:WO0, use TCA_SPLIT_LCMP0EN_bm
    // PA2 is TCA0:WO2, use TCA_SPLIT_LCMP2EN_bm
    // PA3 is TCA0:WO3, use TCA_SPLIT_HCMP0EN_bm
    // PWM is locked by hardware to single-slope fast mode only
    // set split mode
    TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
    // must set period for both counters individually
    TCA0.SPLIT.LPER = PWM_RGB_TOP_INIT;
    TCA0.SPLIT.HPER = PWM_RGB_TOP_INIT;
    // enable the comparators we need
    TCA0.SPLIT.CTRLB = TCA_SPLIT_LCMP0EN_bm
                     | TCA_SPLIT_LCMP2EN_bm
                     | TCA_SPLIT_HCMP0EN_bm;
    // enable and start
    TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV1_gc
                     | TCA_SPLIT_ENABLE_bm;
}

void disable_auxrgb_pwm() {
    // TCA no longer being used, so turn it off
    TCA0.SINGLE.CTRLB = 0;
    TCA0.SINGLE.CTRLA = 0;
    set_auxrgb_power(0);
}

rgb_uint_t get_level_auxrgb(uint8_t level) {
    // convert ramp level to raw PWM value
    if (level) level = PWM3_GET(level - 1);
    return level;
}

void set_auxrgb_pwm(RGB8_t color) {
    if (! TCA0.SINGLE.CTRLA) { enable_auxrgb_pwm(); }
    CH_R_PWM = color.r;
    CH_G_PWM = color.g;
    CH_B_PWM = color.b;
}

bool gradual_adjust_rgb(PWM3_DATATYPE r, PWM3_DATATYPE g, PWM3_DATATYPE b) {
    GRADUAL_ADJUST_SIMPLE(r, CH_R_PWM);
    GRADUAL_ADJUST_SIMPLE(g, CH_G_PWM);
    GRADUAL_ADJUST_SIMPLE(b, CH_B_PWM);

    if ((r == CH_R_PWM)
     && (g == CH_G_PWM)
     && (b == CH_B_PWM)) {
        return true;  // done
    }
    return false;  // not done yet
}

void set_level_hsv(uint8_t level) {
    RGB_t color;
    uint8_t h = cfg.channel_mode_args[channel_mode];
    uint8_t s = 255;  // TODO: drop saturation at brightest levels
    PWM3_DATATYPE v = PWM3_GET(level);
    color = hsv2rgb(h, s, v);

    set_auxrgb_pwm(color);
}

bool gradual_tick_hsv(uint8_t gt) {
    // figure out what exact PWM levels we're aiming for
    RGB_t color;
    uint8_t h = cfg.channel_mode_args[channel_mode];
    uint8_t s = 255;  // TODO: drop saturation at brightest levels
    PWM3_DATATYPE v = PWM3_GET(gt);
    color = hsv2rgb(h, s, v);

    return gradual_adjust_rgb(color.r, color.g, color.b);
}


///// Voltage measurement and weak battery detection

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

    set_level(0);

    // wait a moment so user can tighten the tailcap
    #ifdef WEAK_BATTERY_TEST_DELAY
        for (uint16_t i=0; i<(WEAK_BATTERY_TEST_DELAY * 2 / 3); i++)
            delay_zero();
    #else
        for (uint8_t i=0; i<32; i++) delay_zero();  // wait about 10ms
    #endif

    // baseline unloaded measurement
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

