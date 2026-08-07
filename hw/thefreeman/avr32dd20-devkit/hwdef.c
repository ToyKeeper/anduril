// thefreeman boost driver 2.1 output helper functions
// Copyright (C) 2023-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "anduril/channel-modes.h"  // for circular_tint_3h()
#include "fsm/chan-rgbaux.c"
#include "fsm/ramping.h"

void set_level_zero();

void set_level_main(uint8_t level);
bool gradual_tick_main(uint8_t gt);

void enable_auxrgb_pwm();
void disable_auxrgb_pwm();
rgb_uint_t get_level_auxrgb(uint8_t level);
void set_auxrgb_pwm(RGB8_t color);
void set_level_hsv(uint8_t level);
bool gradual_tick_hsv(uint8_t gt);

Channel channels[] = {
    { // main LEDs
        .set_level    = set_level_main,
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
    DAC_LVL  = 0;  // DAC off
    DAC_VREF = V10;  // low Vref
    HDR_ENABLE_PORT &= ~(1 << HDR_ENABLE_PIN);  // HDR off

    // prevent post-off flash
    IN_NFET_ENABLE_PORT |= (1 << IN_NFET_ENABLE_PIN);
    delay_4ms(IN_NFET_DELAY_TIME/4);
    IN_NFET_ENABLE_PORT &= ~(1 << IN_NFET_ENABLE_PIN);

    #ifdef USE_BST_BYPASS
    // turn off bypass
    BST_BYPASS_PORT |= (1 << BST_BYPASS_PIN);
    #endif

    // turn off boost last
    BST_ENABLE_PORT &= ~(1 << BST_ENABLE_PIN);  // BST off

    // turn off PWM for aux RGB
    disable_auxrgb_pwm();
}

// single set of LEDs with 1 regulated power channel
// and low/high HDR plus low/high Vref as different "gears"
void set_level_main(uint8_t level) {
    uint8_t noflash = 0;

    // when turning on from off, use IN_NFET to prevent a flash
    if ((! actual_level) && (level < HDR_ENABLE_LEVEL_MIN)) {
        noflash = 1;
        IN_NFET_ENABLE_PORT |= (1 << IN_NFET_ENABLE_PIN);
    }

    // BST on first, to give it a few extra microseconds to spin up
    BST_ENABLE_PORT |= (1 << BST_ENABLE_PIN);

    #ifdef USE_BST_BYPASS
    // turn on bypass in li-ion mode
    if (voltage > DUAL_VOLTAGE_FLOOR)
        BST_BYPASS_PORT &= ~(1 << BST_BYPASS_PIN);  // low = bypass
    else  // turn off bypass in AA/NiMH mode
        BST_BYPASS_PORT |= (1 << BST_BYPASS_PIN);  // high = boost
    #endif

    // pre-load ramp data so it can be assigned faster later
    // DAC level register is left-aligned
    PWM1_DATATYPE dac_lvl  = PWM1_GET(level) << 6;
    PWM2_DATATYPE dac_vref = PWM2_GET(level);

    // enable HDR on top half of ramp
    if (level >= (HDR_ENABLE_LEVEL_MIN-1))
        HDR_ENABLE_PORT |= (1 << HDR_ENABLE_PIN);
    else
        HDR_ENABLE_PORT &= ~(1 << HDR_ENABLE_PIN);

    // set these in successive clock cycles to avoid getting out of sync
    // (minimizes ramp bumps when changing gears)
    DAC_LVL  = dac_lvl;
    DAC_VREF = dac_vref;

    if (noflash) {
        // wait for flash prevention to finish
        delay_4ms(IN_NFET_DELAY_TIME/4);
        IN_NFET_ENABLE_PORT &= ~(1 << IN_NFET_ENABLE_PIN);
    }
}

bool gradual_tick_main(uint8_t gt) {
    // if HDR and Vref "engine gear" is the same, do a small adjustment...
    // otherwise, simply jump to the next ramp level
    //   and let set_level() handle any gear changes

    // different gear = full adjustment
    PWM2_DATATYPE vref_next = PWM2_GET(gt);
    if (vref_next != DAC_VREF) return true;  // let parent set_level() for us

    // same gear = small adjustment
    PWM1_DATATYPE dac_now  = DAC_LVL >> 6;  // register is left-aligned
    PWM1_DATATYPE dac_next = PWM1_GET(gt);

    // adjust multiple times based on how far until the next level
    // (so it adjusts faster/coarser for big steps)

    int16_t diff = (dac_next - dac_now);
    if (diff < 0) diff = -diff;

    // ~70 max DAC levels per ramp step, 1 + (70 >> 3) = max 10
    uint8_t steps;
    steps = 1 + (diff >> 3);
    for (uint8_t i=0; i<=steps; i++)
        GRADUAL_ADJUST_SIMPLE(dac_next, dac_now);

    DAC_LVL = dac_now << 6;

    if (dac_next == dac_now) return true;  // done

    return false;  // not done yet
}

///// RGB aux PWM stuff

void enable_auxrgb_pwm() {
    // set up the PWM for aux RGB
    // AVR32_16DD20_14_Prel_DataSheet_DS40002413-2997818.pdf
    // data sheet section 23.4 Register Summary - Normal Mode
    // PA0 is TCA0:WO0, use TCA_SINGLE_CMP0EN_bm
    // PA1 is TCA0:WO1, use TCA_SINGLE_CMP1EN_bm
    // PA2 is TCA0:WO2, use TCA_SINGLE_CMP2EN_bm
    // For Fast (Single Slope) PWM use TCA_SINGLE_WGMODE_SINGLESLOPE_gc
    // For Phase Correct (Dual Slope) PWM use TCA_SINGLE_WGMODE_DSBOTTOM_gc
    // See the manual for other pins, clocks, configs, portmux, etc
    // enable the comparators we need
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm
                      | TCA_SINGLE_CMP1EN_bm
                      | TCA_SINGLE_CMP2EN_bm
                      | TCA_SINGLE_WGMODE_DSBOTTOM_gc;
    // enable and start
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
                      | TCA_SINGLE_ENABLE_bm;
    PWM_RGB_TOP = PWM_RGB_TOP_INIT;
}

void disable_auxrgb_pwm() {
    // TCA no longer being used, so turn it off
    TCA0.SINGLE.CTRLB = 0;
    TCA0.SINGLE.CTRLA = 0;
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


///// Voltage measurement and weak battery detection

uint8_t voltage_raw2cooked(uint16_t measurement) {
    // In : 65535 * BATTLVL / 1.024V
    // Out: uint8_t: Vbat * 50
    // BATTLVL = Vbat * (100.0/(330+100)) = Vbat / 4.3
    // So, Out = In * 4.3 / 1280
    // (plus a bit of fudging to fix the slope and offset,
    //  based on measuring actual hardware)
    uint8_t result = (uint32_t)(measurement + (65535 * 4 / 1024))
                     * 43 / 12880;
    return result;
}

