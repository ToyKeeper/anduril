// BLF LT1S Pro hwdef functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once


void set_level_zero();

void set_level_red(uint8_t level);
void set_level_white_blend(uint8_t level);
void set_level_auto_2ch_blend(uint8_t level);
void set_level_auto_3ch_blend(uint8_t level);
void set_level_red_white_blend(uint8_t level);

bool gradual_tick_red(uint8_t gt);
bool gradual_tick_white_blend(uint8_t gt);
bool gradual_tick_auto_2ch_blend(uint8_t gt);
bool gradual_tick_auto_3ch_blend(uint8_t gt);
bool gradual_tick_red_white_blend(uint8_t gt);


Channel channels[] = {
    { // manual blend of warm and cool white
        .set_level    = set_level_white_blend,
        .gradual_tick = gradual_tick_white_blend,
        .has_args     = 1
    },
    { // auto blend from warm white to cool white
        .set_level    = set_level_auto_2ch_blend,
        .gradual_tick = gradual_tick_auto_2ch_blend,
        .has_args     = 0
    },
    { // auto blend from red to warm white to cool white
        .set_level    = set_level_auto_3ch_blend,
        .gradual_tick = gradual_tick_auto_3ch_blend,
        .has_args     = 0
    },
    { // red only
        .set_level    = set_level_red,
        .gradual_tick = gradual_tick_red,
        .has_args     = 0
    },
    { // manual white blend + adjustable red
        .set_level    = set_level_red_white_blend,
        .gradual_tick = gradual_tick_red_white_blend,
        .has_args     = 1
    }
};


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


void set_level_zero() {
    WARM_PWM_LVL = 0;
    COOL_PWM_LVL = 0;
    RED_PWM_LVL  = 0;
    PWM_CNT      = 0;  // reset phase
}

// single set of LEDs with 1 power channel and dynamic PWM
void set_level_red(uint8_t level) {
    RED_PWM_LVL  = PWM_GET(pwm1_levels, level);
    // pulse frequency modulation, a.k.a. dynamic PWM
    PWM_TOP = PWM_GET(pwm_tops, level);
    // force reset phase when turning on from zero
    // (because otherwise the initial response is inconsistent)
    if (! actual_level) PWM_CNT = 0;
}


// warm + cool blend w/ dynamic PWM
void set_level_white_blend(uint8_t level) {
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    PWM_DATATYPE top        = PWM_GET(pwm_tops, level);
    uint8_t blend           = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, top, blend);

    WARM_PWM_LVL = warm_PWM;
    COOL_PWM_LVL = cool_PWM;
    PWM_TOP = top;
    if (! actual_level) PWM_CNT = 0;  // reset phase
}


// same as white blend, but tint is calculated from the ramp level
void set_level_auto_2ch_blend(uint8_t level) {
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    PWM_DATATYPE top        = PWM_GET(pwm_tops, level);
    uint8_t blend           = 255 * (uint16_t)level / RAMP_SIZE;

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, top, blend);

    WARM_PWM_LVL = warm_PWM;
    COOL_PWM_LVL = cool_PWM;
    PWM_TOP = top;
    if (! actual_level) PWM_CNT = 0;  // reset phase
}


// "auto tint" channel mode with dynamic PWM
void set_level_auto_3ch_blend(uint8_t level) {
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

    PWM_DATATYPE vpwm = PWM_GET(pwm1_levels, level);

    // set the red LED as a ratio of the white output level
    // 0 = no red
    // 255 = red at 100% of white channel PWM
    uint8_t ratio = cfg.channel_mode_args[channel_mode];

    RED_PWM_LVL = (((PWM_DATATYPE2)ratio * (PWM_DATATYPE2)vpwm) + 127) / 255;
    if (! actual_level) PWM_CNT = 0;  // reset phase
}


///// "gradual tick" functions for smooth thermal regulation /////

///// bump each channel toward a target value /////
bool gradual_adjust(uint16_t red, uint16_t warm, uint16_t cool) {
    GRADUAL_ADJUST_SIMPLE(red,  RED_PWM_LVL );
    GRADUAL_ADJUST_SIMPLE(warm, WARM_PWM_LVL);
    GRADUAL_ADJUST_SIMPLE(cool, COOL_PWM_LVL);

    // check for completion
    if ((red  == RED_PWM_LVL )
     && (warm == WARM_PWM_LVL)
     && (cool == COOL_PWM_LVL)) {
        return true;  // done
    }
    return false;  // not done yet
}

bool gradual_tick_red(uint8_t gt) {
    uint16_t red = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(red, 0, 0);
}


bool gradual_tick_white_blend(uint8_t gt) {
    // figure out what exact PWM levels we're aiming for
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, gt);
    PWM_DATATYPE top        = PWM_GET(pwm_tops, gt);
    uint8_t blend           = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, top, blend);

    return gradual_adjust(0, warm_PWM, cool_PWM);
}


// same as white blend, but tint is calculated from the ramp level
bool gradual_tick_auto_2ch_blend(uint8_t gt) {
    // figure out what exact PWM levels we're aiming for
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, gt);
    PWM_DATATYPE top        = PWM_GET(pwm_tops, gt);
    uint8_t blend           = 255 * (uint16_t)gt / RAMP_SIZE;

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, top, blend);

    return gradual_adjust(0, warm_PWM, cool_PWM);
}


bool gradual_tick_auto_3ch_blend(uint8_t gt) {
    // figure out what exact PWM levels we're aiming for
    PWM_DATATYPE red, warm, cool;
    calc_auto_3ch_blend(&red, &warm, &cool, gt);
    return gradual_adjust(red, warm, cool);
}


bool gradual_tick_red_white_blend(uint8_t gt) {
    // figure out what exact PWM levels we're aiming for
    PWM_DATATYPE red, warm, cool;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, gt);
    PWM_DATATYPE top        = PWM_GET(pwm_tops, gt);
    uint8_t blend           = cfg.channel_mode_args[CM_WHITE];
    uint8_t ratio           = cfg.channel_mode_args[channel_mode];

    red = (((PWM_DATATYPE2)ratio * (PWM_DATATYPE2)brightness) + 127) / 255;
    calc_2ch_blend(&warm, &cool, brightness, top, blend);

    return gradual_adjust(red, warm, cool);
}

