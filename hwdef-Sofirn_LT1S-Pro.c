// BLF LT1S Pro hwdef functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once


// calculate a "tint ramp" blend between 2 channels
// results are placed in *warm and *cool vars
// brightness : total amount of light units to distribute
// top : maximum allowed brightness per channel
// blend : ratio between warm and cool (0 = warm, 128 = 50%, 255 = cool)
void calc_2ch_blend(
    PWM_DATATYPE *warm,
    PWM_DATATYPE *cool,
    PWM_DATATYPE brightness,
    PWM_DATATYPE top,
    uint8_t blend) {

    #ifndef TINT_RAMPING_CORRECTION
    #define TINT_RAMPING_CORRECTION 26  // 140% brightness at middle tint
    #endif

    // calculate actual PWM levels based on a single-channel ramp
    // and a blend value
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE2 base_PWM = brightness;

    #if defined(TINT_RAMPING_CORRECTION) && (TINT_RAMPING_CORRECTION > 0)
        uint8_t level = actual_level - 1;

        // middle tints sag, so correct for that effect
        // by adding extra power which peaks at the middle tint
        // (correction is only necessary when PWM is fast)
        if (level > HALFSPEED_LEVEL) {
            base_PWM = brightness
                     + ((((PWM_DATATYPE2)brightness) * TINT_RAMPING_CORRECTION / 64)
                        * triangle_wave(blend) / 255);
        }
        // fade the triangle wave out when above 100% power,
        // so it won't go over 200%
        if (brightness > top) {
            base_PWM -= 2 * (
                             ((brightness - top) * TINT_RAMPING_CORRECTION / 64)
                             * triangle_wave(blend) / 255
                        );
        }
        // guarantee no more than 200% power
        if (base_PWM > (top << 1)) { base_PWM = top << 1; }
    #endif

    cool_PWM = (((PWM_DATATYPE2)blend * (PWM_DATATYPE2)base_PWM) + 127) / 255;
    warm_PWM = base_PWM - cool_PWM;
    // when running at > 100% power, spill extra over to other channel
    if (cool_PWM > top) {
        warm_PWM += (cool_PWM - top);
        cool_PWM = top;
    } else if (warm_PWM > top) {
        cool_PWM += (warm_PWM - top);
        warm_PWM = top;
    }

    *warm = warm_PWM;
    *cool = cool_PWM;
}


// calculate a 3-channel "auto tint" blend
// (like red -> warm white -> cool white)
// results are placed in *a, *b, and *c vars
// level : ramp level to convert into 3 channel levels
// (assumes ramp table is "pwm1_levels")
void calc_auto_3ch_blend(
    PWM_DATATYPE *a,
    PWM_DATATYPE *b,
    PWM_DATATYPE *c,
    uint8_t level) {

    PWM_DATATYPE vpwm = PWM_GET(pwm1_levels, level);

    // tint goes from 0 (red) to 127 (warm white) to 255 (cool white)
    uint8_t mytint;
    mytint = 255 * (uint16_t)level / RAMP_SIZE;

    // red is high at 0, low at 255 (linear)
    *a = (((PWM_DATATYPE2)(255 - mytint)
         * (PWM_DATATYPE2)vpwm) + 127) / 255;
    // warm white is low at 0 and 255, high at 127 (linear triangle)
    *b = (((PWM_DATATYPE2)triangle_wave(mytint)
         * (PWM_DATATYPE2)vpwm) + 127) / 255;
    // cool white is low at 0, high at 255 (linear)
    *c = (((PWM_DATATYPE2)mytint
         * (PWM_DATATYPE2)vpwm) + 127) / 255;

}


// single set of LEDs with 1 power channel and dynamic PWM
void set_level_red(uint8_t level) {
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


// warm + cool blend w/ dynamic PWM
void set_level_white_blend(uint8_t level) {
    if (level == 0) {
        WARM_PWM_LVL = 0;
        COOL_PWM_LVL = 0;
        PWM_CNT      = 0;  // reset phase
        return;
    }

    level --;  // PWM array index = level - 1

    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    PWM_DATATYPE top = PWM_GET(pwm_tops, level);
    uint8_t blend = channel_mode_args[channel_mode];

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, top, blend);

    WARM_PWM_LVL = warm_PWM;
    COOL_PWM_LVL = cool_PWM;
    PWM_TOP = top;
    if (! actual_level) PWM_CNT = 0;  // reset phase
}


// "auto tint" channel mode with dynamic PWM
void set_level_auto_3ch_blend(uint8_t level) {
    if (level == 0) {
        WARM_PWM_LVL = 0;
        COOL_PWM_LVL = 0;
        RED_PWM_LVL  = 0;
        PWM_CNT      = 0;  // reset phase
        return;
    }

    level --;  // PWM array index = level - 1

    PWM_DATATYPE a, b, c;
    calc_auto_3ch_blend(&a, &b, &c, level);

    // pulse frequency modulation, a.k.a. dynamic PWM
    uint16_t top = PWM_GET(pwm_tops, level);

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
    set_level_white_blend(level);
    channel_mode = CM_WHITE_RED;

    if (level == 0) {
        RED_PWM_LVL = 0;
        PWM_CNT     = 0;  // reset phase
        return;
    }

    level --;  // PWM array index = level - 1
    PWM_DATATYPE vpwm = PWM_GET(pwm1_levels, level);

    // set the red LED as a ratio of the white output level
    // 0 = no red
    // 255 = red at 100% of white channel PWM
    uint8_t ratio = channel_mode_args[channel_mode];

    RED_PWM_LVL = (((PWM_DATATYPE2)ratio * (PWM_DATATYPE2)vpwm) + 127) / 255;
    if (! actual_level) PWM_CNT = 0;  // reset phase
}


///// "gradual tick" functions for smooth thermal regulation /////

void gradual_tick_red() {
    GRADUAL_TICK_SETUP();

    GRADUAL_ADJUST_1CH(pwm1_levels, RED_PWM_LVL);

    if ((RED_PWM_LVL  == PWM_GET(pwm1_levels,  gt)))
    {
        GRADUAL_IS_ACTUAL();
    }
}


void gradual_tick_white_blend() {
    uint8_t gt = gradual_target;
    if (gt < actual_level) gt = actual_level - 1;
    else if (gt > actual_level) gt = actual_level + 1;
    gt --;

    // figure out what exact PWM levels we're aiming for
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, gt);
    PWM_DATATYPE top        = PWM_GET(pwm_tops, gt);
    uint8_t blend           = channel_mode_args[channel_mode];

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, top, blend);

    // move up/down if necessary
    GRADUAL_ADJUST_SIMPLE(warm_PWM, WARM_PWM_LVL);
    GRADUAL_ADJUST_SIMPLE(cool_PWM, COOL_PWM_LVL);

    // check for completion
    if (   (WARM_PWM_LVL == warm_PWM)
        && (COOL_PWM_LVL == cool_PWM)
       )
    {
        GRADUAL_IS_ACTUAL();
    }
}


void gradual_tick_auto_3ch_blend() {
    uint8_t gt = gradual_target;
    if (gt < actual_level) gt = actual_level - 1;
    else if (gt > actual_level) gt = actual_level + 1;
    gt --;

    // figure out what exact PWM levels we're aiming for
    PWM_DATATYPE red, warm, cool;
    calc_auto_3ch_blend(&red, &warm, &cool, gt);

    // move up/down if necessary
    GRADUAL_ADJUST_SIMPLE(red,  RED_PWM_LVL);
    GRADUAL_ADJUST_SIMPLE(warm, WARM_PWM_LVL);
    GRADUAL_ADJUST_SIMPLE(cool, COOL_PWM_LVL);

    // check for completion
    if (   (RED_PWM_LVL  == red)
        && (WARM_PWM_LVL == warm)
        && (COOL_PWM_LVL == cool)
       )
    {
        GRADUAL_IS_ACTUAL();
    }
}


void gradual_tick_red_white_blend() {
    // do the white blend thing...
    channel_mode = CM_WHITE;
    gradual_tick_white_blend();
    channel_mode = CM_WHITE_RED;
    // ... and then update red to the closest ramp level
    // (coarse red adjustments aren't visible here anyway)
    set_level_red(actual_level);
}

