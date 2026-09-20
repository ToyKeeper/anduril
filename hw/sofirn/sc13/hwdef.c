// Sofirn SC13 helper functions
// Copyright (C) 2024
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
    CH1_PWM = 0;
    PWM_CNT = 0;  // reset phase
    BCK_ENABLE_PORT &= ~(1 << BCK_ENABLE_PIN);  // disable buck regulator
}

// single set of LEDs with 1 regulated power channels
void set_level_main(uint8_t level) {
    
    BCK_ENABLE_PORT |= (1 << BCK_ENABLE_PIN);  // enable buck regulator
    
    PWM_DATATYPE ch1_pwm = PWM_GET(pwm1_levels, level);
    // pulse frequency modulation, a.k.a. dynamic PWM
    uint16_t top = PWM_GET16(pwm_tops, level);

    CH1_PWM = ch1_pwm;
    // wait to sync the counter and avoid flashes
    // (unnecessary w/ buffered registers)
    //while(actual_level && (PWM_CNT > (top - 32))) {}
    PWM_TOP = top;
    // force reset phase when turning on from zero
    // (because otherwise the initial response is inconsistent)
    if (! actual_level) PWM_CNT = 0;
}

bool gradual_tick_main(uint8_t gt) {
    PWM_DATATYPE pwm1 = PWM_GET(pwm1_levels, gt);

    //GRADUAL_ADJUST_STACKED(pwm1, CH1_PWM, PWM_TOP_INIT);
    GRADUAL_ADJUST_SIMPLE (pwm1, CH1_PWM);

    if (pwm1 == CH1_PWM) return true;  // done

    return false;  // not done yet
}