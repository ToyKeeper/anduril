// BLF LT1S Pro hwdef functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once


// "auto tint" channel mode
void set_level_auto_3ch_blend(uint8_t level) {
    BLEND_PWM_DATATYPE vpwm;

    if (level == 0) {
        vpwm  = 0;
    } else {
        level --;  // PWM array index = level - 1
        vpwm  = PWM_GET(blend_pwm_levels, level);
    }

    // tint goes from 0 (red) to 127 (warm white) to 255 (cool white)
    uint8_t mytint;
    mytint = 255 * (uint16_t)level / RAMP_SIZE;

    BLEND_PWM_DATATYPE a, b, c;

    // red is high at 0, low at 255
    a = (((PWM_DATATYPE2)(255 - mytint)
          * (PWM_DATATYPE2)vpwm) + 127) / 255;
    // warm white is low at 0 and 255, high at 127
    b = (((PWM_DATATYPE2)triangle_wave(mytint)
          * (PWM_DATATYPE2)vpwm) + 127) / 255;
    // cool white is low at 0, high at 255
    c = (((PWM_DATATYPE2)mytint
          * (PWM_DATATYPE2)vpwm) + 127) / 255;

    RED_PWM_LVL  = a;
    WARM_PWM_LVL = b;
    COOL_PWM_LVL = c;
}


// "white + red" channel mode
void set_level_red_white_blend(uint8_t level) {
    // set the warm+cool white LEDs first
    channel_mode = CM_WHITE;
    set_level_2ch_blend(level);
    channel_mode = CM_WHITE_RED;

    BLEND_PWM_DATATYPE vpwm;

    // set the red LED as a ratio of the white output level
    if (level == 0) {
        vpwm  = 0;
    } else {
        level --;  // PWM array index = level - 1
        vpwm  = PWM_GET(blend_pwm_levels, level);
    }

    // 0 = no red
    // 255 = red at 100% of white channel PWM
    uint8_t ratio = channel_mode_args[channel_mode];

    PWM_DATATYPE red_pwm;
    red_pwm = (((PWM_DATATYPE2)ratio * (PWM_DATATYPE2)vpwm) + 127) / 255;

    RED_PWM_LVL = red_pwm;
}

