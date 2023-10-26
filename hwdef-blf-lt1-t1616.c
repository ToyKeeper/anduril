// Sofirn LT1-t1616 PWM helpers
// Copyright (C) 2023 SiteRelEnby, Selene ToyKeeper
// (adapted from emisar-2ch 15/10/2023)
#pragma once

#include "chan-aux.c"


void set_level_zero();

void set_level_ch1(uint8_t level);
void set_level_ch2(uint8_t level);
void set_level_both(uint8_t level);
void set_level_blend(uint8_t level);
void set_level_auto(uint8_t level);

bool gradual_tick_ch1(uint8_t gt);
bool gradual_tick_ch2(uint8_t gt);
bool gradual_tick_both(uint8_t gt);
bool gradual_tick_blend(uint8_t gt);
bool gradual_tick_auto(uint8_t gt);


Channel channels[] = {
    { // channel 1 only
        .set_level    = set_level_ch1,
        .gradual_tick = gradual_tick_ch1,
        .has_args     = 0
    },
    { // channel 2 only
        .set_level    = set_level_ch2,
        .gradual_tick = gradual_tick_ch2,
        .has_args     = 0
    },
    { // both channels, tied together (max "200%" power)
        .set_level    = set_level_both,
        .gradual_tick = gradual_tick_both,
        .has_args     = 0
    },
    { // both channels, manual blend (max "100%" power)
        .set_level    = set_level_blend,
        .gradual_tick = gradual_tick_blend,
        .has_args     = 1
    },
    { // both channels, auto blend
        .set_level    = set_level_auto,
        .gradual_tick = gradual_tick_auto,
        .has_args     = 1
    },
    AUX_CHANNELS
};


void set_level_zero() {
    CH1_PWM = 0;
    CH2_PWM = 0;
}

void set_pwms(PWM_DATATYPE ch1_pwm, PWM_DATATYPE ch2_pwm) {
    CH1_PWM = ch1_pwm;
    CH2_PWM = ch2_pwm;
}

void set_level_ch1(uint8_t level) {
    set_pwms(PWM_GET(pwm1_levels, level), 0);
}

void set_level_ch2(uint8_t level) {
    set_pwms(0, PWM_GET(pwm1_levels, level));
}

void set_level_both(uint8_t level) {
    PWM_DATATYPE pwm = PWM_GET(pwm1_levels, level);
    set_pwms(pwm, pwm);
}

void set_level_blend(uint8_t level) {
    PWM_DATATYPE ch1_pwm, ch2_pwm;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    uint8_t blend           = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&ch1_pwm, &ch2_pwm, brightness, PWM_TOP_INIT, blend);

    set_pwms(ch1_pwm, ch2_pwm);
}

void set_level_auto(uint8_t level) {
    PWM_DATATYPE ch1_pwm, ch2_pwm;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    uint8_t blend           = 255 * (uint16_t)level / RAMP_SIZE;
    if (cfg.channel_mode_args[channel_mode] & 0b01000000)
        blend = 255 - blend;

    calc_2ch_blend(&ch1_pwm, &ch2_pwm, brightness, PWM_TOP_INIT, blend);

    set_pwms(ch1_pwm, ch2_pwm);
}


///// bump each channel toward a target value /////
bool gradual_adjust(uint16_t ch1_pwm, uint16_t ch2_pwm) {
    GRADUAL_ADJUST_SIMPLE(ch1_pwm, CH1_PWM);
    GRADUAL_ADJUST_SIMPLE(ch2_pwm, CH2_PWM);

    // check for completion
    if ((ch1_pwm == CH1_PWM)
     && (ch2_pwm == CH2_PWM)) {
        return true;  // done
    }
    return false;  // not done yet
}

bool gradual_tick_ch1(uint8_t gt) {
    PWM_DATATYPE pwm = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(pwm, 0);
}

bool gradual_tick_ch2(uint8_t gt) {
    PWM_DATATYPE pwm = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(0, pwm);
}

bool gradual_tick_both(uint8_t gt) {
    PWM_DATATYPE pwm = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(pwm, pwm);
}

bool gradual_tick_blend(uint8_t gt) {
    PWM_DATATYPE ch1_pwm, ch2_pwm;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, gt);
    uint8_t blend           = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&ch1_pwm, &ch2_pwm, brightness, PWM_TOP_INIT, blend);

    return gradual_adjust(ch1_pwm, ch2_pwm);
}

bool gradual_tick_auto(uint8_t gt) {
    PWM_DATATYPE ch1_pwm, ch2_pwm;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, gt);
    uint8_t blend           = 255 * (uint16_t)gt / RAMP_SIZE;
    if (cfg.channel_mode_args[channel_mode] & 0b01000000)
        blend = 255 - blend;

    calc_2ch_blend(&ch1_pwm, &ch2_pwm, brightness, PWM_TOP_INIT, blend);

    return gradual_adjust(ch1_pwm, ch2_pwm);
}

