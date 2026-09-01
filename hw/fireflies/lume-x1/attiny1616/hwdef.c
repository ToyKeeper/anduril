// Fireflies Lume-X1 helper functions
// Copyright (C) 2017-2026 Selene ToyKeeper
//               2021-2023 loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "anduril/channel-modes.h"  // for circular_tint_3h()
#include "fsm/chan-rgbaux.c"

uint8_t is_boost_currently_on = 0;  // for turn-on delay during first turn on

void set_level_zero();

void set_level_udr(uint8_t level);
bool gradual_tick_main(uint8_t gt);
void set_power_path(uint8_t ramp_level);

void enable_auxrgb_pwm();
void disable_auxrgb_pwm();
rgb_uint_t get_level_auxrgb(uint8_t level);
void set_auxrgb_pwm(RGB8_t color);
void set_level_hsv(uint8_t level);
bool gradual_tick_hsv(uint8_t gt);

Channel channels[] = {
    { // main LEDs
        .set_level    = set_level_udr,
        .gradual_tick = gradual_tick_main,
        .flags        = 0
    },
    { // aux RGB LEDs
        .set_level    = set_level_hsv,
        .gradual_tick = gradual_tick_hsv,
        .flags        = CHANNEL_FLAG_IS_AUX | CHANNEL_FLAG_HAS_ARGS
    },
    AUXRGB_CHANNELS
};

// HSV mode needs a different 3H handler
StatePtr channel_3H_modes[NUM_CHANNEL_MODES] = {
    NULL, circular_tint_3h,
};

void set_level_zero() {
    DAC_LVL = 0;  // DAC off
    mcu_set_dac_vref(V055);  // low Vref

    // turn off DC/DC converter and amplifier
    BST_ENABLE_PORT &= ~(1 << BST_ENABLE_PIN);
    is_boost_currently_on = 0;

    // turn off all UDR paths
    LED_PATH1_PORT &= ~LED_PATH1_PIN;
    LED_PATH2_PORT &= ~LED_PATH2_PIN;
    LED_PATH3_PORT &= ~LED_PATH3_PIN;

    // turn off PWM for aux RGB
    disable_auxrgb_pwm();
}

// UDR for set_level, which sets the led brightness based on ramp tables.
// single set of LEDs, regulated boost at all levels
void set_level_udr(uint8_t level) {
    if (level == actual_level - 1) return;  //  no-op

    // get the ramp data
    PWM1_DATATYPE dac_lvl  = PWM1_GET(level);
    PWM2_DATATYPE dac_vref = PWM2_GET(level);

    if (is_boost_currently_on != 1) {
        // regulator is not on, enable regulator and add boot-up delay
        is_boost_currently_on = 1;
        BST_ENABLE_PORT |= (1 << BST_ENABLE_PIN);   // turn on regulator and amplifier
        delay_4ms(BST_ON_DELAY/4);                  // boot-up delay
    }

    // set the DAC
    DAC_LVL  = dac_lvl;
    mcu_set_dac_vref(dac_vref);

    // ... and the power paths
    set_power_path(level);
}

// handles dynamic Vref used in the ramp tables
bool gradual_tick_main(uint8_t gt) {
    // TODO overall smoothness can be improved due to gt using linear
    // adjustments, but ramp table is non-linear.

    // if Vref is the same, make gradual adjustments.
    // else, jump to the next ramp level and use set_level() to handle power paths.
    // different gear = full adjustment
    PWM2_DATATYPE vref_next = PWM2_GET(gt);
    // let parent set_level() for us
    if (vref_next != (DAC_VREF & VREF_DAC0REFSEL_gm)) return true;

    // same gear = small adjustment
    PWM1_DATATYPE dac_next = PWM1_GET(gt);
    GRADUAL_ADJUST_SIMPLE(dac_next, DAC_LVL);
    if (dac_next == DAC_LVL) return true;  // done

    return false;  // not done yet
}

// handles dynamic power pathways based on threshold levels
void set_power_path(uint8_t ramp_level) {
    ramp_level ++;  // convert to 1-based indexing

    if (ramp_level >= LED_PATH3_PIN_LEVEL_MIN) {
        // high mode
        LED_PATH1_PORT |=  LED_PATH1_PIN;
        LED_PATH2_PORT |=  LED_PATH2_PIN;
        LED_PATH3_PORT |=  LED_PATH3_PIN;
    }
    else if (ramp_level >= LED_PATH2_PIN_LEVEL_MIN) {
        // low mode
        LED_PATH1_PORT |=  LED_PATH1_PIN;
        LED_PATH2_PORT |=  LED_PATH2_PIN;
        LED_PATH3_PORT &= ~LED_PATH3_PIN;
    }
    else if (ramp_level >= LED_PATH1_PIN_LEVEL_MIN) {
        // firefly mode
        LED_PATH1_PORT |=  LED_PATH1_PIN;
        LED_PATH2_PORT &= ~LED_PATH2_PIN;
        LED_PATH3_PORT &= ~LED_PATH3_PIN;
    }
}


///// RGB aux PWM stuff

void enable_auxrgb_pwm() {
    set_auxrgb_power(0);

    // set up the PWM for aux RGB
    // ATtiny1614-16-17-DataSheet-DS40002204A.pdf
    // enable TCA0 for the green + blue channels
    // data sheet section 20 - TCA - 16-bit Timer/Counter Type A
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm
                      | TCA_SINGLE_CMP1EN_bm
                      | TCA_SINGLE_WGMODE_DSBOTTOM_gc;
    // enable and start
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
                      | TCA_SINGLE_ENABLE_bm;
    PWM_GB_TOP = PWM_RGB_TOP_INIT;
    // enable TCB0 for red channel
    // data sheet section 21 - TCB - 16-bit Timer/Counter Type B
    // enable PWM (ds 21.5.2)
    TCB0.CTRLB = TCB_CNTMODE_gm  // 8-bit PWM mode
               | TCB_CCMPEN_bm;  // enable output
    // sync with TCA0 (ds 21.5.1) and start
    TCB0.CTRLA = TCB_CLKSEL_1_bm  // sync start from TCA0 (note, ds has wrong value)
               | TCB_SYNCUPD_bm  // sync reset from TCA0
               | TCB_ENABLE_bm;  // start
    PWM_R_TOP  = PWM_RGB_TOP_INIT;
}

void disable_auxrgb_pwm() {
    // TCA/TCB no longer being used, so turn it off
    TCA0.SINGLE.CTRLB = 0;
    TCA0.SINGLE.CTRLA = 0;
    TCB0.CTRLA = 0;
    TCB0.CTRLB = 0;
    set_auxrgb_power(0);
}

rgb_uint_t get_level_auxrgb(uint8_t level) {
    // convert ramp level to raw PWM value
    if (level) level = PWM3_GET(level - 1);
    return level;
}

void set_auxrgb_pwm(RGB8_t color) {
    if (! TCA0.SINGLE.CTRLA) { enable_auxrgb_pwm(); }
    CH_R_PWM = color.r;
    CH_G_PWM = color.g;
    CH_B_PWM = color.b;
}

bool gradual_adjust_rgb(PWM3_DATATYPE r, PWM3_DATATYPE g, PWM3_DATATYPE b) {
    GRADUAL_ADJUST_SIMPLE(r, CH_R_PWM);
    GRADUAL_ADJUST_SIMPLE(g, CH_G_PWM);
    GRADUAL_ADJUST_SIMPLE(b, CH_B_PWM);

    if ((r == CH_R_PWM)
     && (g == CH_G_PWM)
     && (b == CH_B_PWM)) {
        return true;  // done
    }
    return false;  // not done yet
}

void set_level_hsv(uint8_t level) {
    RGB_t color;
    uint8_t h = cfg.channel_mode_args[channel_mode];
    uint8_t s = 255;  // TODO: drop saturation at brightest levels
    PWM3_DATATYPE v = PWM3_GET(level);
    color = hsv2rgb(h, s, v);

    set_auxrgb_pwm(color);
}

bool gradual_tick_hsv(uint8_t gt) {
    // figure out what exact PWM levels we're aiming for
    RGB_t color;
    uint8_t h = cfg.channel_mode_args[channel_mode];
    uint8_t s = 255;  // TODO: drop saturation at brightest levels
    PWM3_DATATYPE v = PWM3_GET(gt);
    color = hsv2rgb(h, s, v);

    return gradual_adjust_rgb(color.r, color.g, color.b);
}

