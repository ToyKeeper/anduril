// Emisar 2-channel-plus-FET-joined-into-1-channel
// Copyright (C) 2024 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fsm/chan-rgbaux.c"


void set_level_zero();

void set_level_ch1(uint8_t level);
void set_level_ch1_fet(uint8_t level);

bool gradual_tick_ch1(uint8_t gt);
bool gradual_tick_ch1_fet(uint8_t gt);


Channel channels[] = {
    { // channel 1 only
        .set_level    = set_level_ch1,
        .gradual_tick = gradual_tick_ch1,
        // .has_args     = 0
    },
    { // channel 1 + DD FET
        .set_level    = set_level_ch1_fet,
        .gradual_tick = gradual_tick_ch1_fet,
        // .has_args     = 0
    },
    RGB_AUX_CHANNELS
};


void set_level_zero() {
    // disable timer overflow interrupt
    // (helps improve button press handling from Off state)
    DSM_INTCTRL &= ~DSM_OVF_bm;

    // turn off all LEDs
    ch1_dsm_lvl = 0;
    //ch2_dsm_lvl = 0;
    CH1_PWM     = 0;
    CH2_PWM     = 0;
    CH3_PWM     = 0;
    PWM_CNT     = 0;  // reset phase
    CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable opamp
    CH2_ENABLE_PORT &= ~(1 << CH2_ENABLE_PIN);  // disable opamp
}


// wrap setting the dsm vars, to get a faster response
// (just setting *_dsm_lvl doesn't work well for strobes)
// set new values for both channels,
// handling any possible combination
// and any before/after state
void set_hw_levels(PWM1_DATATYPE ch1,
                   //PWM2_DATATYPE ch2,
                   PWM3_DATATYPE ch3
                   //, bool ch1_on, bool ch2_on
                   ) {

    //bool was_on = (CH1_PWM>0) || (CH2_PWM>0)
    //            || ( CH1_ENABLE_PORT & (1 << CH1_ENABLE_PIN) )
    //            || ( CH2_ENABLE_PORT & (1 << CH2_ENABLE_PIN) );

    //if (ch1 || ch1_on)
        CH1_ENABLE_PORT |= (1 << CH1_ENABLE_PIN);  // enable opamp
    //else
    //    CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable opamp

    //if (ch2 || ch2_on)
    //    CH2_ENABLE_PORT |= (1 << CH2_ENABLE_PIN);  // enable opamp
    //else
    //    CH2_ENABLE_PORT &= ~(1 << CH2_ENABLE_PIN);  // disable opamp

    // set delta-sigma soft levels
    ch1_dsm_lvl = ch1;
    //ch2_dsm_lvl = ch2;

    // set hardware PWM levels and init dsm loop
    CH1_PWM = ch1_pwm = ch1 >> 7;
    //CH2_PWM = ch2_pwm = ch2 >> 7;
    CH3_PWM = ch3;

    // enable timer overflow interrupt so DSM can work
    DSM_INTCTRL |= DSM_OVF_bm;

    // reset phase when turning on
    if (! prev_level) PWM_CNT = 0;

}

// delta-sigma modulation of PWM outputs
// happens on each Timer overflow (every 512 cpu clock cycles)
// uses 8-bit pwm w/ 7-bit dsm (0b 0PPP PPPP PDDD DDDD)
ISR(DSM_vect) {
    // set new hardware values first,
    // for best timing (reduce effect of interrupt jitter)
    CH1_PWM = ch1_pwm;
    //CH2_PWM = ch2_pwm;

    // calculate next values, now that timing matters less

    // accumulate error
    ch1_dsm += (ch1_dsm_lvl & 0x007f);
    // next PWM = base PWM value + carry bit
    ch1_pwm  = (ch1_dsm_lvl >> 7) + (ch1_dsm > 0x7f);
    // clear carry bit
    ch1_dsm &= 0x7f;

    // repeat for other channels

    //ch2_dsm += (ch2_dsm_lvl & 0x007f);
    //ch2_pwm  = (ch2_dsm_lvl >> 7) + (ch2_dsm > 0x7f);
    //ch2_dsm &= 0x7f;
}


void set_level_ch1(uint8_t level) {
    PWM1_DATATYPE pwm1 = PWM1_GET(level);
    set_hw_levels(pwm1, 0);
}

void set_level_ch1_fet(uint8_t level) {
    PWM2_DATATYPE pwm2 = PWM2_GET(level);
    PWM3_DATATYPE pwm3 = PWM3_GET(level);
    set_hw_levels(pwm2, pwm3);
}


///// bump each channel toward a target value /////
bool gradual_adjust(PWM1_DATATYPE ch1,
                    //PWM2_DATATYPE ch2,
                    PWM3_DATATYPE ch3) {

    // instant 0 to TOP change, and TOP to 0
    if (((DSM_TOP == ch1_dsm_lvl) && (0 == ch1)) ||
        ((DSM_TOP == ch1) && (0 == ch1_dsm_lvl)))
        ch1_dsm_lvl = ch1;

    // bump the DD FET multiple steps
    // (it only has 255 steps, but the effect is small
    //  compared to the linear channel it's stacked on,
    //  so smaller steps tend to overheat on turbo)
    for (uint8_t i=0; i<4; i++)
        GRADUAL_ADJUST_SIMPLE(ch3, CH3_PWM);

    // if DSM changed by less than 100%,
    // adjust multiple times based on current brightness
    // (so it adjusts faster/coarser when bright, slower/finer when dim)

    // higher shift = slower/finer adjustments
    const uint8_t shift = 9;  // ((255 << 7) >> 9) = 63 max
    uint8_t steps;

    steps = ch1_dsm_lvl >> shift;
    for (uint8_t i=0; i<=steps; i++)
        GRADUAL_ADJUST_SIMPLE(ch1, ch1_dsm_lvl);

    //steps = ch2_dsm_lvl  >> shift;
    ////for (uint8_t i=0; i<=steps; i++)
    ////    GRADUAL_ADJUST_SIMPLE(ch2, ch2_dsm_lvl);

    if ((ch1 == ch1_dsm_lvl)
     //&& (ch2 == ch2_dsm_lvl)
     && (ch3 == CH3_PWM) ) {
        return true;  // done
    }
    return false;  // not done yet
}

bool gradual_tick_ch1(uint8_t gt) {
    PWM1_DATATYPE pwm1 = PWM1_GET(gt);
    return gradual_adjust(pwm1, 0);
}

bool gradual_tick_ch1_fet(uint8_t gt) {
    PWM2_DATATYPE pwm2 = PWM2_GET(gt);
    PWM3_DATATYPE pwm3 = PWM3_GET(gt);
    return gradual_adjust(pwm2, pwm3);
}

