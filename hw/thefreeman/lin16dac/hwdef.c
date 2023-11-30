// thefreeman linear t1616 DAC driver helper functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fsm/chan-aux.c"

void set_level_zero();

void set_level_main(uint8_t level);
bool gradual_tick_main(uint8_t gt);


Channel channels[] = {
    { // main LEDs
        .set_level    = set_level_main,
        .gradual_tick = gradual_tick_main
    },
    AUX_CHANNELS
};


void set_level_zero() {
    DAC_LVL  = 0;  // DAC off
    mcu_set_dac_vref(V055);  // low Vref
    HDR_ENABLE_PORT &= ~(1 << HDR_ENABLE_PIN);  // HDR off

    // prevent post-off flash
    //IN_NFET_ENABLE_PORT |= (1 << IN_NFET_ENABLE_PIN);
    //delay_4ms(IN_NFET_DELAY_TIME/4);
    //IN_NFET_ENABLE_PORT &= ~(1 << IN_NFET_ENABLE_PIN);

    // turn off opamp last
    OPAMP_ENABLE_PORT &= ~(1 << OPAMP_ENABLE_PIN);  // Opamp off
}

// single set of LEDs with 1 regulated power channel
// and low/high HDR plus low/high Vref as different "gears"
void set_level_main(uint8_t level) {
    #if 0  // unsure if this helps anything
    uint8_t noflash = 0;

    // when turning on from off, try to prevent a flash
    if ((! actual_level) && (level < HDR_ENABLE_LEVEL_MIN)) {
        noflash = 1;
    }
    #endif

    // Opamp on first, to give it a few extra microseconds to spin up
    OPAMP_ENABLE_PORT |= (1 << OPAMP_ENABLE_PIN);

    // pre-load ramp data so it can be assigned faster later
    PWM1_DATATYPE dac_lvl  = PWM1_GET(level);
    PWM2_DATATYPE dac_vref = PWM2_GET(level);

    // enable HDR on top half of ramp
    if (level >= (HDR_ENABLE_LEVEL_MIN-1))
        HDR_ENABLE_PORT |= (1 << HDR_ENABLE_PIN);
    else
        HDR_ENABLE_PORT &= ~(1 << HDR_ENABLE_PIN);

    #if 0
    if (noflash) {
        // wait for flash prevention to finish
        delay_4ms(OPAMP_ON_DELAY/4);
    }
    #endif

    // set these in successive clock cycles to avoid getting out of sync
    // (minimizes ramp bumps when changing gears)
    DAC_LVL  = dac_lvl;
    mcu_set_dac_vref(dac_vref);

}

bool gradual_tick_main(uint8_t gt) {
    // if HDR and Vref "engine gear" is the same, do a small adjustment...
    // otherwise, simply jump to the next ramp level
    //   and let set_level() handle any gear changes

    PWM1_DATATYPE dac_next  = PWM1_GET(gt);
    PWM2_DATATYPE vref_next = PWM2_GET(gt);

    // different gear = full adjustment
    if (vref_next != (DAC_VREF & VREF_DAC0REFSEL_gm)) return true;  // let parent set_level() for us

    // same gear = small adjustment
    GRADUAL_ADJUST_SIMPLE(dac_next, DAC_LVL);
    if (dac_next == DAC_LVL) return true;  // done

    return false;  // not done yet
}

