// Fireflies Lume10 helper functions
// Copyright (C) 2017-2026 Selene ToyKeeper
//               2021-2024 loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "anduril/channel-modes.h"  // for circular_tint_3h()
#include "fsm/chan-rgbaux.c"

uint8_t is_buck_currently_on = 0;  // for turn-on delay during first turn on

void set_level_zero();

void set_level_udrf(uint8_t level);
bool gradual_tick_main(uint8_t gt);
void set_power_path(uint8_t ramp_level);

rgb_uint_t get_level_auxrgb(uint8_t level);
void set_level_hsv(uint8_t level);
bool gradual_tick_hsv(uint8_t gt);

Channel channels[] = {
    { // main LEDs
        .set_level    = set_level_udrf,
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
    //FET_LVL  = 0;  // FET off
    // turn off FET
    FET_PORT.OUTCLR = (1 << FET_PIN);
    DAC_LVL  = 0;  // DAC off
    DAC_VREF = V10;  // set DAC Vref to lowest

    // turn off DC/DC converter and amplifier
    BCK_ENABLE_PORT &= ~(1 << BCK_ENABLE_PIN);
    is_buck_currently_on = 0;

    // turn off all UDR paths
    LED_PATH1_PORT &= ~LED_PATH1_PIN;
    LED_PATH2_PORT &= ~LED_PATH2_PIN;
    LED_PATH3_PORT &= ~LED_PATH3_PIN;

    // turn off PWM for aux RGB
    disable_auxrgb_pwm();
}

// UDR+FET for set_level, which sets the led brightness based on ramp tables.
// single set of LED(s) with 2 separate power channels, regulated buck + FET,
// only one is active at a time
void set_level_udrf(uint8_t level) {
    if (level == actual_level - 1) return;  //  no-op

    // weird driver, turbo is 1 level only
    if (level >= (RAMP_SIZE - 1)) {  // levels are 0..149, RAMP_SIZE is 150
        // fet is on, disable buck converter
        BCK_ENABLE_PORT &= ~(1 << BCK_ENABLE_PIN);
        is_buck_currently_on = 0;
        DAC_LVL = 0;
        DAC_VREF = V10;
        // turn path 3 on to reduce DCR
        LED_PATH3_PORT |= LED_PATH3_PIN;
        // turn on FET
        FET_PORT.OUTSET = (1 << FET_PIN);
        // ... and we're done
        return;
    }

    // everything *except* turbo below

    // turn off FET
    FET_PORT.OUTCLR = (1 << FET_PIN);

    // get the ramp data
    PWM1_DATATYPE dac_lvl  = PWM1_GET(level) << 6;  // dac register is left-aligned
    PWM2_DATATYPE dac_vref = PWM2_GET(level);

    if (! is_buck_currently_on) {
        // regulator is not on, enable regulator and add boot-up delay
        DAC_LVL = 0; // initialize as 0
        is_buck_currently_on = 1;
        BCK_ENABLE_PORT |= (1 << BCK_ENABLE_PIN);   // turn on regulator and amplifier
        delay_4ms(BCK_ON_DELAY/4);                  // boot-up delay
    }

    // set the DAC
    DAC_LVL  = dac_lvl;
    DAC_VREF = dac_vref;

    // ... and the power paths
    set_power_path(level);
}

// handles dynamic Vref used in the ramp tables
bool gradual_tick_main(uint8_t gt) {
    // TODO overall smoothness can be improved due to gt using linear
    // adjustments, but ramp table is non-linear.

    // let parent handle FET levels.. don't adjust gradually
    if (gt >= (RAMP_SIZE - 2)) return true;

    // if Vref is the same, make gradual adjustments.
    // else, jump to the next ramp level and use set_level() to handle power paths.
    // different gear = full adjustment
    PWM2_DATATYPE vref_next = PWM2_GET(gt);
    if (vref_next != DAC_VREF) return true;  // let parent set_level() for us

    // same gear = small adjustment
    PWM1_DATATYPE dac_now  = DAC_LVL >> 6;  // register is left-aligned
    PWM1_DATATYPE dac_next = PWM1_GET(gt);

    // only adjust 1 dac level, max is 1023
    // (but speed it up with "#define GRADUAL_ADJUST_SPEED  4" elsewhere)
    GRADUAL_ADJUST_SIMPLE(dac_next, dac_now);

    DAC_LVL = dac_now << 6;

    if (dac_next == dac_now) return true;  // done

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

rgb_uint_t get_level_auxrgb(uint8_t level) {
    // convert ramp level to raw PWM value
    if (level) level = PWM3_GET(level - 1);
    return level;
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

    return gradual_adjust_auxrgb(color);
}

void hwdef_EV_temperature_high(int16_t howmuch) {
    // the single-level DD FET turbo makes thermal regulation weird
    // so override the thermal warning behavior a bit
    if (actual_level >= RAMP_SIZE) {  // stepping down from turbo
        // step down only 1 level
        emit(EV_temperature_high, 1);
        // reset temperature history to pause adjustments for a few seconds
        // (temporarily removes predictions as a factor)
        adc_reset = 1;
    } else {  // already below turbo
        emit(EV_temperature_high, howmuch);
    }
}

