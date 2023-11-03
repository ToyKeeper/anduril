// Emisar 2-channel generic w/ tint ramping
// Copyright (C) 2021-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fsm/chan-rgbaux.c"


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
    RGB_AUX_CHANNELS
};


// set new values for both channels,
// handling any possible combination
// and any before/after state
void set_pwms(uint16_t ch1_pwm, uint16_t ch2_pwm, uint16_t top) {
    bool was_on = (CH1_PWM>0) | (CH2_PWM>0);
    bool now_on = (ch1_pwm>0) | (ch2_pwm>0);

    if (! now_on) {
        CH1_PWM = 0;
        CH2_PWM = 0;
        PWM_TOP = PWM_TOP_INIT;
        PWM_CNT = 0;
        CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable opamp
        CH2_ENABLE_PORT &= ~(1 << CH2_ENABLE_PIN);  // disable opamp
        return;
    }

    if (ch1_pwm)
        CH1_ENABLE_PORT |= (1 << CH1_ENABLE_PIN);  // enable opamp
    else
        CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable opamp

    if (ch2_pwm)
        CH2_ENABLE_PORT |= (1 << CH2_ENABLE_PIN);  // enable opamp
    else
        CH2_ENABLE_PORT &= ~(1 << CH2_ENABLE_PIN);  // disable opamp

    CH1_PWM = ch1_pwm;
    CH2_PWM = ch2_pwm;

    // manual phase sync when changing level while already on
    if (was_on && now_on) while(PWM_CNT > (top - 32)) {}

    PWM_TOP = top;

    // reset phase when turning on or off
    //if ((! was_on) | (! now_on)) PWM_CNT = 0;
    if (! was_on) PWM_CNT = 0;
}

void set_level_zero() {
    return set_pwms(0, 0, PWM_TOP_INIT);
}

void set_level_ch1(uint8_t level) {
    uint16_t pwm = PWM_GET(pwm1_levels, level);
    uint16_t top = PWM_GET(pwm_tops, level);
    set_pwms(pwm, 0, top);
}

void set_level_ch2(uint8_t level) {
    uint16_t pwm = PWM_GET(pwm1_levels, level);
    uint16_t top = PWM_GET(pwm_tops, level);
    set_pwms(0, pwm, top);
}

void set_level_both(uint8_t level) {
    uint16_t pwm = PWM_GET(pwm1_levels, level);
    uint16_t top = PWM_GET(pwm_tops, level);
    set_pwms(pwm, pwm, top);
}

void set_level_blend(uint8_t level) {
    PWM_DATATYPE ch1_pwm, ch2_pwm;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    PWM_DATATYPE top        = PWM_GET(pwm_tops, level);
    uint8_t blend           = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&ch1_pwm, &ch2_pwm, brightness, top, blend);

    set_pwms(ch1_pwm, ch2_pwm, top);
}

void set_level_auto(uint8_t level) {
    PWM_DATATYPE ch1_pwm, ch2_pwm;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    PWM_DATATYPE top        = PWM_GET(pwm_tops, level);
    uint8_t blend           = 255 * (uint16_t)level / RAMP_SIZE;
    if (cfg.channel_mode_args[channel_mode] & 0b01000000)
        blend = 255 - blend;

    calc_2ch_blend(&ch1_pwm, &ch2_pwm, brightness, top, blend);

    set_pwms(ch1_pwm, ch2_pwm, top);
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
    uint16_t pwm = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(pwm, 0);
}

bool gradual_tick_ch2(uint8_t gt) {
    uint16_t pwm = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(0, pwm);
}

bool gradual_tick_both(uint8_t gt) {
    uint16_t pwm = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(pwm, pwm);
}

bool gradual_tick_blend(uint8_t gt) {
    PWM_DATATYPE ch1_pwm, ch2_pwm;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, gt);
    PWM_DATATYPE top        = PWM_GET(pwm_tops, gt);
    uint8_t blend           = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&ch1_pwm, &ch2_pwm, brightness, top, blend);

    return gradual_adjust(ch1_pwm, ch2_pwm);
}

bool gradual_tick_auto(uint8_t gt) {
    PWM_DATATYPE ch1_pwm, ch2_pwm;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, gt);
    PWM_DATATYPE top        = PWM_GET(pwm_tops, gt);
    uint8_t blend           = 255 * (uint16_t)gt / RAMP_SIZE;
    if (cfg.channel_mode_args[channel_mode] & 0b01000000)
        blend = 255 - blend;

    calc_2ch_blend(&ch1_pwm, &ch2_pwm, brightness, top, blend);

    return gradual_adjust(ch1_pwm, ch2_pwm);
}


