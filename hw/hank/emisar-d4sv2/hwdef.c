// Emisar D4Sv2 PWM helper functions
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "fsm/chan-rgbaux.c"

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
    CH1_PWM = 0;
    CH2_PWM = 0;
    CH3_PWM = 0;
    PWM_CNT = 0;  // reset phase
}

// single set of LEDs with 3 stacked power channels, DDFET+3+1
void set_level_main(uint8_t level) {
    PWM_DATATYPE ch1_pwm = PWM_GET(pwm1_levels, level);
    PWM_DATATYPE ch2_pwm = PWM_GET(pwm2_levels, level);
    PWM_DATATYPE ch3_pwm = PWM_GET(pwm3_levels, level);
    // pulse frequency modulation, a.k.a. dynamic PWM
    uint16_t top = PWM_GET16(pwm_tops, level);

    CH1_PWM = ch1_pwm;
    CH2_PWM = ch2_pwm;
    CH3_PWM = ch3_pwm;
    // wait to sync the counter and avoid flashes
    while(actual_level && (PWM_CNT > (top - 32))) {}
    PWM_TOP = top;
    // force reset phase when turning on from zero
    // (because otherwise the initial response is inconsistent)
    if (! actual_level) PWM_CNT = 0;
}

bool gradual_tick_main(uint8_t gt) {
    PWM_DATATYPE pwm1 = PWM_GET(pwm1_levels, gt);
    PWM_DATATYPE pwm2 = PWM_GET(pwm2_levels, gt);
    PWM_DATATYPE pwm3 = PWM_GET(pwm3_levels, gt);

    GRADUAL_ADJUST_STACKED(pwm1, CH1_PWM, PWM_TOP_INIT);
    GRADUAL_ADJUST_STACKED(pwm2, CH2_PWM, PWM_TOP_INIT);
    GRADUAL_ADJUST_SIMPLE (pwm3, CH3_PWM);

    if (   (pwm1 == CH1_PWM)
        && (pwm2 == CH2_PWM)
        && (pwm3 == CH3_PWM)
       ) {
        return true;  // done
    }
    return false;  // not done yet
}

