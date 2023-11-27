// FW3X Lume1 helper functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fsm/chan-rgbaux.c"


void set_level_zero();

void set_level_main(uint8_t level);
bool gradual_tick_main(uint8_t gt);


Channel channels[] = {
    { // channel 1 only
        .set_level    = set_level_main,
        .gradual_tick = gradual_tick_main
    },
    RGB_AUX_CHANNELS
};


void set_level_zero() {
    // disable timer overflow interrupt
    // (helps improve button press handling from Off state)
    DSM_INTCTRL &= ~DSM_OVF_bm;

    // turn off all LEDs
    ch1_dsm_lvl = 0;
    CH1_PWM = 0;
    CH2_PWM = 0;
    PWM_CNT = 0;  // reset phase
    CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable regulator
}

// single set of LEDs with 2 stacked power channels, regulated + DD FET
void set_level_main(uint8_t level) {
    if (level == actual_level - 1) return;  // prevent flicker on no-op

    PWM1_DATATYPE ch1 = PWM1_GET(level);
    PWM2_DATATYPE ch2 = PWM2_GET(level);

    // set delta-sigma soft levels
    ch1_dsm_lvl = ch1;

    // set hardware PWM levels and init dsm loop
    CH1_PWM = ch1_pwm = ch1 >> 7;
    CH2_PWM =           ch2;

    // enable timer overflow interrupt so DSM can work
    DSM_INTCTRL |= DSM_OVF_bm;

    // force reset phase when turning on from zero
    // (for faster, more consistent initial response)
    if (! actual_level) PWM_CNT = 0;

    // don't enable ch1 and ch2 at the same time
    if (ch2) CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable regulator
    else CH1_ENABLE_PORT |= (1 << CH1_ENABLE_PIN);  // enable regulator
}

// delta-sigma modulation of PWM outputs
// happens on each Timer overflow (every 512 cpu clock cycles)
// uses 8-bit pwm w/ 7-bit dsm (0b 0PPP PPPP PDDD DDDD)
ISR(DSM_vect) {
    // set new hardware values first,
    // for best timing (reduce effect of interrupt jitter)
    CH1_PWM = ch1_pwm;

    // calculate next values, now that timing matters less

    // accumulate error
    ch1_dsm += (ch1_dsm_lvl & 0x007f);
    // next PWM = base PWM value + carry bit
    ch1_pwm  = (ch1_dsm_lvl >> 7) + (ch1_dsm > 0x7f);
    // clear carry bit
    ch1_dsm &= 0x7f;
}


bool gradual_tick_main(uint8_t gt) {
    // 150/150 is full FET + zero regulated,
    // 149/150 is zero FET + full regulated,
    // so don't try to gradually adjust between
    if ((RAMP_SIZE == actual_level) || (gt >= RAMP_SIZE-1)) {
        set_level(gt + 1);
        return true;
    }

    PWM1_DATATYPE ch1 = PWM1_GET(gt);

    // adjust multiple times based on current brightness
    // (so it adjusts faster/coarser when bright, slower/finer when dim)

    // higher shift = slower/finer adjustments
    const uint8_t shift = 9;  // ((255 << 7) >> 9) = 63 max
    uint8_t steps;

    steps = ch1_dsm_lvl >> shift;
    for (uint8_t i=0; i<=steps; i++)
        GRADUAL_ADJUST_SIMPLE(ch1, ch1_dsm_lvl);

    if ((ch1 == ch1_dsm_lvl)
       ) {
        return true;  // done
    }
    return false;  // not done yet
}

