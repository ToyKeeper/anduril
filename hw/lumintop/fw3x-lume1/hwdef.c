// FW3X Lume1 PWM helper functions
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
    CH1_PWM = 0;
    CH2_PWM = 0;
    PWM_CNT = 0;  // reset phase
    CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable regulator
}

// single set of LEDs with 2 stacked power channels, regulated + DD FET
void set_level_main(uint8_t level) {
    CH1_ENABLE_PORT |= (1 << CH1_ENABLE_PIN);  // enable regulator

    PWM_DATATYPE ch1_pwm = PWM_GET(pwm1_levels, level);
    PWM_DATATYPE ch2_pwm = PWM_GET(pwm2_levels, level);

    CH1_PWM = ch1_pwm;
    CH2_PWM = ch2_pwm;

    // force reset phase when turning on from zero
    // (for faster, more consistent initial response)
    if (! actual_level) PWM_CNT = 0;
}

bool gradual_tick_main(uint8_t gt) {
    // 150/150 is full FET + zero regulated,
    // 149/150 is zero FET + full regulated,
    // so don't try to gradually adjust between
    if ((RAMP_SIZE == actual_level) || (gt >= RAMP_SIZE-1)) {
        set_level(gt + 1);
        return true;
    }

    PWM_DATATYPE pwm1 = PWM_GET(pwm1_levels, gt);
    GRADUAL_ADJUST_SIMPLE(pwm1, CH1_PWM);

    if (pwm1 == CH1_PWM) return true;  // done
    return false;  // not done yet
}

