// Noctigon KR4 (no DD FET) PWM helper functions
// Copyright (C) 2020-2023 Selene ToyKeeper
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
    CH2_PWM = 0;
    PWM_CNT = 0;  // reset phase
    CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable opamp
}

// single set of LEDs with linear power channel
void set_level_main(uint8_t level) {
    CH1_ENABLE_PORT |= (1 << CH1_ENABLE_PIN);  // enable opamp

    PWM_DATATYPE ch1_pwm = PWM_GET(pwm1_levels, level);
    // pulse frequency modulation, a.k.a. dynamic PWM
    uint16_t top = PWM_GET16(pwm_tops, level);

    CH1_PWM = ch1_pwm;
    CH2_PWM = 0;
    // wait to sync the counter and avoid flashes
    while(actual_level && (PWM_CNT > (top - 32))) {}
    PWM_TOP = top;
    // force reset phase when turning on from zero
    // (because otherwise the initial response is inconsistent)
    if (! actual_level) PWM_CNT = 0;
}

bool gradual_tick_main(uint8_t gt) {
    PWM_DATATYPE pwm1 = PWM_GET(pwm1_levels, gt);

    GRADUAL_ADJUST_SIMPLE(pwm1, CH1_PWM);

    if (   (pwm1 == CH1_PWM)
       ) {
        return true;  // done
    }
    return false;  // not done yet
}

