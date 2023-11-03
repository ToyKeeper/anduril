// hwdef for Noctigon M44 2-channel light
// Copyright (C) 2023 Selene ToyKeeper
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


void set_level_zero() {
    // disable timer overflow interrupt
    // (helps improve button press handling from Off state)
    DSM_INTCTRL &= ~DSM_OVF_bm;

    // turn off all LEDs
    ch1_dsm_lvl = 0;
    ch2_dsm_lvl = 0;
    CH1_PWM     = 0;
    CH2_PWM     = 0;
    PWM_CNT     = 0;
    CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable opamp
    CH2_ENABLE_PORT &= ~(1 << CH2_ENABLE_PIN);  // disable opamp
}


// wrap setting the dsm vars, to get a faster response
// (just setting *_dsm_lvl doesn't work well for strobes)
// set new values for both channels,
// handling any possible combination
// and any before/after state
void set_hw_levels(PWM_DATATYPE ch1, PWM_DATATYPE ch2,
                   bool ch1_on,      bool ch2_on) {

    bool was_on = (CH1_PWM>0) || (CH2_PWM>0)
                || ( CH1_ENABLE_PORT & (1 << CH1_ENABLE_PIN) )
                || ( CH2_ENABLE_PORT & (1 << CH2_ENABLE_PIN) );
    //bool now_on = (ch1>0) || (ch2>0) || ch1_on || ch2_on;

    #if 0  // not needed any more, after switching to PWM+DSM
    // phase-correct PWM at zero (for flicker-free moon),
    // fast PWM otherwise
    if (ch1 || ch2)
        TCCR1B  = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
                | (1<<WGM13)  | (1<<WGM12)  // fast adjustable PWM (DS table 12-5)
                //| (1<<WGM13)  | (0<<WGM12)  // phase-correct adjustable PWM (DS table 12-5)
                ;
    else
        TCCR1B  = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
                //| (1<<WGM13)  | (1<<WGM12)  // fast adjustable PWM (DS table 12-5)
                | (1<<WGM13)  | (0<<WGM12)  // phase-correct adjustable PWM (DS table 12-5)
                ;
    #endif

    if (ch1 || ch1_on)
        CH1_ENABLE_PORT |= (1 << CH1_ENABLE_PIN);  // enable opamp
    else
        CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable opamp

    if (ch2 || ch2_on)
        CH2_ENABLE_PORT |= (1 << CH2_ENABLE_PIN);  // enable opamp
    else
        CH2_ENABLE_PORT &= ~(1 << CH2_ENABLE_PIN);  // disable opamp

    // set delta-sigma soft levels
    ch1_dsm_lvl = ch1;
    ch2_dsm_lvl = ch2;

    // set hardware PWM levels and init dsm loop
    CH1_PWM = ch1_pwm = ch1 >> 7;
    CH2_PWM = ch2_pwm = ch2 >> 7;

    #if 0  // not needed any more, after switching to PWM+DSM
    // manual phase sync when changing level while already on
    if (was_on && now_on) while(PWM_CNT > (top - 32)) {}

    PWM_TOP = top;
    #endif

    // enable timer overflow interrupt so DSM can work
    DSM_INTCTRL |= DSM_OVF_bm;

    // reset phase when turning on
    //if ((! was_on) | (! now_on)) PWM_CNT = 0;
    if (! was_on) PWM_CNT = 0;

}

// delta-sigma modulation of PWM outputs
// happens on each Timer overflow (every 512 cpu clock cycles)
// uses 8-bit pwm w/ 7-bit dsm (0b 0PPP PPPP PDDD DDDD)
ISR(DSM_vect) {
    // set new hardware values first,
    // for best timing (reduce effect of interrupt jitter)
    CH1_PWM = ch1_pwm;
    CH2_PWM = ch2_pwm;

    // calculate next values, now that timing matters less

    // accumulate error
    ch1_dsm += (ch1_dsm_lvl & 0x007f);
    // next PWM = base PWM value + carry bit
    ch1_pwm  = (ch1_dsm_lvl >> 7) + (ch1_dsm > 0x7f);
    // clear carry bit
    ch1_dsm &= 0x7f;

    // repeat for other channels

    ch2_dsm += (ch2_dsm_lvl & 0x007f);
    ch2_pwm  = (ch2_dsm_lvl >> 7) + (ch2_dsm > 0x7f);
    ch2_dsm &= 0x7f;
}


void set_level_ch1(uint8_t level) {
    PWM_DATATYPE pwm = PWM_GET(pwm1_levels, level);
    set_hw_levels(pwm, 0,
                  1,   0);
}

void set_level_ch2(uint8_t level) {
    PWM_DATATYPE pwm = PWM_GET(pwm1_levels, level);
    set_hw_levels(0, pwm,
                  0, 1);
}

void set_level_both(uint8_t level) {
    PWM_DATATYPE pwm = PWM_GET(pwm1_levels, level);
    set_hw_levels(pwm, pwm,
                  1,   1);
}

void blend_helper(PWM_DATATYPE *warm, PWM_DATATYPE *cool, uint8_t level) {
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    uint8_t blend;
    if (channel_mode == CM_AUTO) {
        blend = 255 * (uint16_t)level / RAMP_SIZE;
        if (cfg.channel_mode_args[channel_mode] & 0b01000000)
            blend = 255 - blend;
    } else {
        blend = cfg.channel_mode_args[channel_mode];
    }

    calc_2ch_blend(warm, cool, brightness, DSM_TOP, blend);
}

void set_level_blend(uint8_t level) {
    PWM_DATATYPE warm, cool;
    uint8_t blend = cfg.channel_mode_args[channel_mode];
    blend_helper(&warm, &cool, level);
    // don't turn off either emitter entirely while using middle blends
    set_hw_levels(warm,        cool,
                  blend < 255, blend > 0);
}

void set_level_auto(uint8_t level) {
    PWM_DATATYPE warm, cool;
    blend_helper(&warm, &cool, level);
    // don't turn off either emitter entirely
    // (it blinks pretty bright when the regulator turns on mid-ramp)
    set_hw_levels(warm, cool,
                  1,    1);
}

///// "gradual tick" functions for smooth thermal regulation /////
// (and other smooth adjustments)

///// bump each channel toward a target value /////
bool gradual_adjust(PWM_DATATYPE ch1, PWM_DATATYPE ch2) {
    // adjust multiple times based on current brightness
    // (so it adjusts faster/coarser when bright, slower/finer when dim)

    // higher shift = slower/finer adjustments
    const uint8_t shift = 9;  // ((255 << 7) >> 9) = 63 max
    uint8_t steps;

    steps = ch1_dsm_lvl >> shift;
    for (uint8_t i=0; i<=steps; i++)
        GRADUAL_ADJUST_SIMPLE(ch1, ch1_dsm_lvl);

    steps = ch2_dsm_lvl  >> shift;
    for (uint8_t i=0; i<=steps; i++)
        GRADUAL_ADJUST_SIMPLE(ch2, ch2_dsm_lvl);

    if ((ch1 == ch1_dsm_lvl)
     && (ch2 == ch2_dsm_lvl )) {
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
    PWM_DATATYPE warm, cool;
    blend_helper(&warm, &cool, gt);
    return gradual_adjust(warm, cool);
}

bool gradual_tick_auto(uint8_t gt) {
    PWM_DATATYPE warm, cool;
    blend_helper(&warm, &cool, gt);
    return gradual_adjust(warm, cool);
}


