// Emisar D4 PWM helper functions
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

//#ifdef AUXLED_PIN
#if 0
#include "fsm/chan-aux.c"
#else
#define AUX_CHANNELS
#endif

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
    CH1_PWM = 0;
    CH2_PWM = 0;
}

// TODO: implement delta-sigma modulation for better low modes

// single set of LEDs with 2 stacked power channels, DDFET+1 or DDFET+linear
void set_level_main(uint8_t level) {
    PWM_DATATYPE ch1_pwm = PWM_GET(pwm1_levels, level);
    PWM_DATATYPE ch2_pwm = PWM_GET(pwm2_levels, level);

    CH1_PWM = ch1_pwm;
    CH2_PWM = ch2_pwm;
}

bool gradual_tick_main(uint8_t gt) {
    PWM_DATATYPE pwm1 = PWM_GET(pwm1_levels, gt);
    PWM_DATATYPE pwm2 = PWM_GET(pwm2_levels, gt);

    GRADUAL_ADJUST_STACKED(pwm1, CH1_PWM, PWM_TOP_INIT);
    GRADUAL_ADJUST_SIMPLE (pwm2, CH2_PWM);

    if (   (pwm1 == CH1_PWM)
        && (pwm2 == CH2_PWM)
       ) {
        return true;  // done
    }
    return false;  // not done yet
}

