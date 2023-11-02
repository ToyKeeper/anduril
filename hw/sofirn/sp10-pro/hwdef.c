// Sofirn SP10 Pro PWM helper functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

void set_level_zero();

void set_level_main(uint8_t level);
bool gradual_tick_main(uint8_t gt);


Channel channels[] = {
    { // main LEDs
        .set_level    = set_level_main,
        .gradual_tick = gradual_tick_main
    },
};


void set_level_zero() {
    CH1_PWM = 0;
    CH2_PWM = 0;
    PWM_CNT = 0;  // reset phase
    BST_ENABLE_PORT &= ~(1 << BST_ENABLE_PIN);  // boost off
}

// single set of LEDs with 2 stacked power channels
void set_level_main(uint8_t level) {
    PWM_DATATYPE ch1_pwm = PWM_GET(pwm1_levels, level);
    PWM_DATATYPE ch2_pwm = PWM_GET(pwm2_levels, level);
    // pulse frequency modulation, a.k.a. dynamic PWM
    uint16_t top = PWM_GET16(pwm_tops, level);

    BST_ENABLE_PORT |= (1 << BST_ENABLE_PIN);  // boost on

    CH1_PWM = ch1_pwm;
    CH2_PWM = ch2_pwm;
    PWM_TOP = top;

    // force reset phase when turning on from zero
    // (because otherwise the initial response is inconsistent)
    if (! actual_level) PWM_CNT = 0;
}

bool gradual_tick_main(uint8_t gt) {
    PWM_DATATYPE pwm1 = PWM_GET(pwm1_levels, gt);
    PWM_DATATYPE pwm2 = PWM_GET(pwm2_levels, gt);

    // ch1 sometimes makes huge leaps; don't make it gradual
    // if either current or new level is in the leap zone, just leap
    if ((CH1_PWM + pwm1) > 128) CH1_PWM = pwm1;
    else GRADUAL_ADJUST_SIMPLE(pwm1, CH1_PWM);

    GRADUAL_ADJUST_SIMPLE(pwm2, CH2_PWM);

    if (   (pwm1 == CH1_PWM)
        && (pwm2 == CH2_PWM)
       ) {
        return true;  // done
    }
    return false;  // not done yet
}

