// BLF LT1S Pro hwdef functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once


// single set of LEDs with 1 power channel and dynamic PWM
void set_level_1ch_dyn(uint8_t level) {
    if (level == 0) {
        RED_PWM_LVL  = 0;
        PWM_CNT      = 0;  // reset phase
    } else {
        level --;  // PWM array index = level - 1
        RED_PWM_LVL  = PWM_GET(pwm1_levels, level);
        // pulse frequency modulation, a.k.a. dynamic PWM
        PWM_TOP = PWM_GET(pwm_tops, level);
        // force reset phase when turning on from zero
        // (because otherwise the initial response is inconsistent)
        if (! actual_level) PWM_CNT = 0;
    }
}


// warm + cool blend w/ middle sag correction and dynamic PWM
void set_level_2ch_dyn_blend(uint8_t level) {
    #ifndef TINT_RAMPING_CORRECTION
    #define TINT_RAMPING_CORRECTION 26  // 140% brightness at middle tint
    #endif

    if (level == 0) {
        WARM_PWM_LVL = 0;
        COOL_PWM_LVL = 0;
        PWM_CNT      = 0;  // reset phase
        return;
    }

    level --;  // PWM array index = level - 1
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    uint16_t top = PWM_GET(pwm_tops, level);

    // calculate actual PWM levels based on a single-channel ramp
    // and a global tint value
    uint16_t warm_PWM, cool_PWM;
    uint8_t mytint = channel_mode_args[channel_mode];

    PWM_DATATYPE2 base_PWM = brightness;
    #if defined(TINT_RAMPING_CORRECTION) && (TINT_RAMPING_CORRECTION > 0)
        // middle tints sag, so correct for that effect
        // by adding extra power which peaks at the middle tint
        // (correction is only necessary when PWM is fast)
        if (level > HALFSPEED_LEVEL) {
            base_PWM = brightness
                     + ((((PWM_DATATYPE2)brightness) * TINT_RAMPING_CORRECTION / 64)
                        * triangle_wave(mytint) / 255);
        }
        // fade the triangle wave out when above 100% power,
        // so it won't go over 200%
        if (brightness > top) {
            base_PWM -= 2 * (
                             ((brightness - top) * TINT_RAMPING_CORRECTION / 64)
                             * triangle_wave(mytint) / 255
                        );
        }
        // guarantee no more than 200% power
        if (base_PWM > (top << 1)) { base_PWM = top << 1; }
    #endif

    cool_PWM = (((PWM_DATATYPE2)mytint * (PWM_DATATYPE2)base_PWM) + 127) / 255;
    warm_PWM = base_PWM - cool_PWM;
    // when running at > 100% power, spill extra over to other channel
    if (cool_PWM > top) {
        warm_PWM += (cool_PWM - top);
        cool_PWM = top;
    } else if (warm_PWM > top) {
        cool_PWM += (warm_PWM - top);
        warm_PWM = top;
    }

    WARM_PWM_LVL = warm_PWM;
    COOL_PWM_LVL = cool_PWM;
    PWM_TOP = top;
    if (! actual_level) PWM_CNT = 0;  // reset phase
}


// "auto tint" channel mode with dynamic PWM
void set_level_auto_3ch_dyn_blend(uint8_t level) {
    if (level == 0) {
        WARM_PWM_LVL = 0;
        COOL_PWM_LVL = 0;
        RED_PWM_LVL  = 0;
        PWM_CNT      = 0;  // reset phase
        return;
    }

    level --;  // PWM array index = level - 1
    PWM_DATATYPE vpwm = PWM_GET(pwm1_levels, level);
    // pulse frequency modulation, a.k.a. dynamic PWM
    uint16_t top = PWM_GET(pwm_tops, level);

    // tint goes from 0 (red) to 127 (warm white) to 255 (cool white)
    uint8_t mytint;
    mytint = 255 * (uint16_t)level / RAMP_SIZE;

    PWM_DATATYPE a, b, c;

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
    PWM_TOP = top;
    if (! actual_level) PWM_CNT = 0;
}


// "white + red" channel mode
void set_level_red_white_blend(uint8_t level) {
    // set the warm+cool white LEDs first
    channel_mode = CM_WHITE;
    set_level_2ch_dyn_blend(level);
    channel_mode = CM_WHITE_RED;

    // set the red LED as a ratio of the white output level
    if (level == 0) {
        RED_PWM_LVL = 0;
        PWM_CNT     = 0;  // reset phase
        return;
    }

    level --;  // PWM array index = level - 1
    PWM_DATATYPE vpwm = PWM_GET(pwm1_levels, level);

    // 0 = no red
    // 255 = red at 100% of white channel PWM
    uint8_t ratio = channel_mode_args[channel_mode];

    PWM_DATATYPE red_pwm;
    red_pwm = (((PWM_DATATYPE2)ratio * (PWM_DATATYPE2)vpwm) + 127) / 255;

    RED_PWM_LVL = red_pwm;
    if (! actual_level) PWM_CNT = 0;  // reset phase
}

