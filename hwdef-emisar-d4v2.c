// Emisar D4v2 PWM helper functions
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "chan-rgbaux.c"

// single set of LEDs with 2 stacked power channels, DDFET+1 or DDFET+linear
void set_level_stacked(uint8_t level) {
    if (level == 0) {
        LOW_PWM_LVL  = 0;
        HIGH_PWM_LVL = 0;
        PWM_CNT      = 0;  // reset phase
    } else {
        level --;  // PWM array index = level - 1
        LOW_PWM_LVL  = PWM_GET(pwm1_levels, level);
        HIGH_PWM_LVL = PWM_GET(pwm2_levels, level);
        // pulse frequency modulation, a.k.a. dynamic PWM
        uint16_t top = PWM_GET(pwm_tops, level);
        // wait to sync the counter and avoid flashes
        while(actual_level && (PWM_CNT > (top - 32))) {}
        PWM_TOP = top;
        // force reset phase when turning on from zero
        // (because otherwise the initial response is inconsistent)
        if (! actual_level) PWM_CNT = 0;
    }
}

bool gradual_tick_stacked(uint8_t gt) {
    GRADUAL_TICK_SETUP();

    GRADUAL_ADJUST(pwm1_levels, LOW_PWM_LVL, PWM_TOP_INIT);
    GRADUAL_ADJUST_1CH(pwm2_levels, HIGH_PWM_LVL);

    // did we go far enough to hit the next defined ramp level?
    // if so, update the main ramp level tracking var
    if (   (LOW_PWM_LVL  == PWM_GET(pwm1_levels, gt))
        && (HIGH_PWM_LVL == PWM_GET(pwm2_levels, gt))
       ) {
        return true;
    }
    return false;
}

