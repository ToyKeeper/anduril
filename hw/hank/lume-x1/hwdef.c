// Copyright (C) 2017-2023 Selene ToyKeeper
//               2021-2024 loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later

//***********************************************
//**  HELPER FUNCTIONS FOR LUME-X1-AVR32DD20   **
//***********************************************

#pragma once

#include "anduril/channel-modes.h"  // for circular_tint_3h()
#include "fsm/chan-rgbaux.c"

// Declare variables and functions to support UDR multiple power paths
uint8_t is_boost_currently_on = 0;   // for turn-on delay during first turn on

void set_level_zero();
void set_level_udr(uint8_t level);
bool gradual_tick_main(uint8_t gt);
void set_power_path(uint8_t ramp_level);

void enable_aux_rgb_pwm();
void disable_aux_rgb_pwm();
void set_rgb(uint8_t r, uint8_t g, uint8_t b);
void set_level_hsv(uint8_t level);
bool gradual_tick_hsv(uint8_t gt);

Channel channels[] = {
    { // main LEDs
        .set_level    = set_level_udr,
        .gradual_tick = gradual_tick_main,
        .has_args = 0
    },
    { // aux RGB LEDs
        .set_level    = set_level_hsv,
        .gradual_tick = gradual_tick_hsv,
        .has_args = 1
    },
    RGB_AUX_CHANNELS
};

// HSV mode needs a different 3H handler
StatePtr channel_3H_modes[NUM_CHANNEL_MODES] = {
    NULL, circular_tint_3h,
};

// turn off
void set_level_zero() {
    DAC_LVL  = 0;           // set DAC to 0
    DAC_VREF = V10;         // set DAC Vref to lowest

    // turn off DC/DC converter and amplifier
    BST_ENABLE_PORT &= ~(1 << BST_ENABLE_PIN);
    is_boost_currently_on = 0;

    // turn off all UDR paths
    LED_PATH1_PORT &= ~LED_PATH1_PIN;
    LED_PATH2_PORT &= ~LED_PATH2_PIN;
    LED_PATH3_PORT &= ~LED_PATH3_PIN;

    // turn off PWM for aux RGB
    disable_aux_rgb_pwm();
}

// UDR for set_level, which sets the led brightness based on ramp tables.
// single set of LED(s), fully regulated boost at all levels
void set_level_udr(uint8_t level) {
    if (level == actual_level - 1) return;  //  no-op

    // get the ramp data
    PWM1_DATATYPE dac_lvl  = PWM1_GET(level) << 6;  // dac register is left-aligned
    PWM2_DATATYPE dac_vref = PWM2_GET(level);

    if(is_boost_currently_on != 1){
        // boost is not on, enable buck and add boot-up delay
        is_boost_currently_on = 1;
        BST_ENABLE_PORT |= (1 << BST_ENABLE_PIN);   // turn on buck and amplifier
        delay_4ms(BST_ON_DELAY/4);                  // boot-up delay
    }
    // set the DAC
    DAC_LVL  = dac_lvl;
    DAC_VREF = dac_vref;

    // set the power paths
    set_power_path(level);
}

// handles dynamic Vref used in the ramp tables
bool gradual_tick_main(uint8_t gt) {
    // TODO overall smoothness can be improved due to gt using linear
    // adjustments, but ramp table is non-linear.

    // if Vref is the same, make gradual adjustments.
    // else, jump to the next ramp level and use set_level() to handle power paths.
    PWM2_DATATYPE vref_next = PWM2_GET(gt); // DAC ramp table Vref

    // if different vref level, make a ramp level adjustment..
    if (vref_next != DAC_VREF) return true;  // use set_level() to handle normally

    // .. else, same vref, adjust level gradually.
    PWM1_DATATYPE dac_next  = PWM1_GET(gt); // DAC ramp table data
    PWM1_DATATYPE dac_curr  = DAC_LVL >> 6; // register is left-aligned

    GRADUAL_ADJUST_SIMPLE(dac_next, dac_curr);
    DAC_LVL = dac_curr << 6;

    if (dac_next == dac_curr) return true;  // done

    return false;
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

void enable_aux_rgb_pwm() {
    TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
    TCA0.SPLIT.LPER = PWM_RGB_TOP_INIT;
    TCA0.SPLIT.HPER = PWM_RGB_TOP_INIT;
    TCA0.SPLIT.CTRLB = TCA_SPLIT_LCMP1EN_bm
                     | TCA_SPLIT_LCMP2EN_bm
                     | TCA_SPLIT_HCMP0EN_bm;
    TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV1_gc
                     | TCA_SPLIT_ENABLE_bm;
}

void disable_aux_rgb_pwm() {
    TCA0.SINGLE.CTRLB = 0;
    TCA0.SINGLE.CTRLA = 0;
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    CH_R_PWM = r;
    CH_G_PWM = g;
    CH_B_PWM = b;
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

    enable_aux_rgb_pwm();
    set_rgb(color.r, color.g, color.b);
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

