// Emisar D4K 3-channel hwdef
// Copyright (C) 2023-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "anduril/channel-modes.h"  //for circular_tint_3h()
#include "fsm/chan-rgbaux.c"


void set_level_zero();

void set_level_main2(uint8_t level);
void set_level_led3(uint8_t level);
void set_level_led4(uint8_t level);
void set_level_all(uint8_t level);
void set_level_led34a_blend(uint8_t level);
void set_level_led34b_blend(uint8_t level);
void set_level_hsv(uint8_t level);
void set_level_auto3(uint8_t level);
void set_level_chaos(uint8_t level);

bool gradual_tick_main2(uint8_t gt);
bool gradual_tick_led3(uint8_t gt);
bool gradual_tick_led4(uint8_t gt);
bool gradual_tick_all(uint8_t gt);
bool gradual_tick_led34a_blend(uint8_t gt);
bool gradual_tick_led34b_blend(uint8_t gt);
bool gradual_tick_hsv(uint8_t gt);
bool gradual_tick_auto3(uint8_t gt);
bool gradual_tick_chaos(uint8_t gt);
uint8_t chaos_3h(Event event, uint16_t arg);


Channel channels[] = {
    { // main 2 LEDs only
        .set_level    = set_level_main2,
        .gradual_tick = gradual_tick_main2,
        .flags        = 0
    },
    { // 3rd LED only
        .set_level    = set_level_led3,
        .gradual_tick = gradual_tick_led3,
        .flags        = 0
    },
    { // 4th LED only
        .set_level    = set_level_led4,
        .gradual_tick = gradual_tick_led4,
        .flags        = 0
    },
    { // all channels, tied together (equal amounts, max power)
        .set_level    = set_level_all,
        .gradual_tick = gradual_tick_all,
        .flags        = 0
    },
    { // 3rd + 4th LEDs, manual blend (max "100%" power) (8/16/16)
        .set_level    = set_level_led34a_blend,
        .gradual_tick = gradual_tick_led34a_blend,
        .flags        = CHANNEL_FLAG_HAS_ARGS
    },
    { // 3rd + 4th LEDs, manual blend (max "100%" power) (16/16/8)
        .set_level    = set_level_led34b_blend,
        .gradual_tick = gradual_tick_led34b_blend,
        .flags        = CHANNEL_FLAG_HAS_ARGS
    },
    { // 3ch blend (HSV style)
        .set_level    = set_level_hsv,
        .gradual_tick = gradual_tick_hsv,
        .flags        = CHANNEL_FLAG_HAS_ARGS
    },
    { // 3ch auto blend (red-warm-cool style, led4-led3-main2)
        .set_level    = set_level_auto3,
        .gradual_tick = gradual_tick_auto3,
        .flags        = 0
    },
    { // chaotic pendulum color animation
        .set_level    = set_level_chaos,
        .gradual_tick = gradual_tick_chaos,
        .flags        = CHANNEL_FLAG_HAS_ARGS
    },
    AUXRGB_CHANNELS
};

// HSV and chaos modes need different 3H handlers
StatePtr channel_3H_modes[NUM_CHANNEL_MODES] = {
    NULL, NULL, NULL, NULL,
    NULL, NULL, circular_tint_3h, NULL, chaos_3h,
};

void set_level_zero() {
    // disable timer overflow interrupt
    // (helps improve button press handling from Off state)
    DSM_INTCTRL &= ~DSM_OVF_bm;

    // turn off all LEDs
    MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);
    LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN );
    LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN );
    main2_dsm_lvl = 0;
    led3_dsm_lvl  = 0;
    led4_dsm_lvl  = 0;
    MAIN2_PWM_LVL = 0;
    LED3_PWM_LVL  = 0;
    LED4_PWM_LVL  = 0;
    PWM_CNT       = 0;
    //PWM_TOP       = PWM_TOP_INIT;
}

// wrap setting the dsm vars, to get a faster response
// (just setting *_dsm_lvl doesn't work well for strobes)
void set_hw_levels(PWM_DATATYPE main2, // brightness, 0 to DSM_TOP
                   PWM_DATATYPE led3,
                   PWM_DATATYPE led4,
                   bool main2_en,      // enable even at PWM=0?
                   bool led3_en,
                   bool led4_en
                   ) {

    // enable/disable LED power channels
    if (main2 | main2_en)
         MAIN2_ENABLE_PORT |=  (1 << MAIN2_ENABLE_PIN);
    else MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);

    if (led3  | led3_en )
         LED3_ENABLE_PORT  |=  (1 << LED3_ENABLE_PIN);
    else LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN);

    if (led4  | led4_en )
         LED4_ENABLE_PORT  |=  (1 << LED4_ENABLE_PIN);
    else LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN);

    // set delta-sigma soft levels
    main2_dsm_lvl = main2;
    led3_dsm_lvl  = led3;
    led4_dsm_lvl  = led4;
    // set hardware PWM levels and init dsm loop
    MAIN2_PWM_LVL = main2_pwm = main2 >> 7;
    LED3_PWM_LVL  = led3_pwm  = led3  >> 7;
    LED4_PWM_LVL  = led4_pwm  = led4  >> 7;

    // enable timer overflow interrupt so DSM can work
    DSM_INTCTRL |= DSM_OVF_bm;

    // force phase reset
    PWM_CNT       = PWM_CNT2  = 0;
}

// delta-sigma modulation of PWM outputs
// happens on each Timer overflow (every 512 cpu clock cycles)
// uses 8-bit pwm w/ 7-bit dsm (0b 0PPP PPPP PDDD DDDD)
ISR(DSM_vect) {
    // set new hardware values first,
    // for best timing (reduce effect of interrupt jitter)
    MAIN2_PWM_LVL = main2_pwm;
    LED3_PWM_LVL  = led3_pwm;
    LED4_PWM_LVL  = led4_pwm;

    // calculate next values, now that timing matters less

    // accumulate error
    main2_dsm += (main2_dsm_lvl & 0x007f);
    // next PWM = base PWM value + carry bit
    main2_pwm  = (main2_dsm_lvl >> 7) + (main2_dsm > 0x7f);
    // clear carry bit
    main2_dsm &= 0x7f;

    // repeat for other channels

    led3_dsm += (led3_dsm_lvl & 0x007f);
    led3_pwm  = (led3_dsm_lvl >> 7) + (led3_dsm > 0x7f);
    led3_dsm &= 0x7f;

    led4_dsm += (led4_dsm_lvl & 0x007f);
    led4_pwm  = (led4_dsm_lvl >> 7) + (led4_dsm > 0x7f);
    led4_dsm &= 0x7f;
}

// LEDs 1+2 are 8-bit
// this 8-bit channel may be LEDs 1+2 or LED 4, depending on wiring
void set_level_main2(uint8_t level) {
    set_hw_levels(PWM_GET(pwm1_levels, level), 0, 0,
                  1,                           0, 0);
}

// LED 3 is 16-bit
void set_level_led3(uint8_t level) {
    set_hw_levels(0, PWM_GET(pwm1_levels, level), 0,
                  0, 1,                           0);
}

// this 16-bit channel may be LED 4 or LEDs 1+2, depending on wiring
void set_level_led4(uint8_t level) {
    set_hw_levels(0, 0, PWM_GET(pwm1_levels, level),
                  0, 0, 1);
}

void set_level_all(uint8_t level) {
    PWM_DATATYPE pwm = PWM_GET(pwm1_levels, level);
    set_hw_levels(pwm, pwm, pwm, 1, 1, 1);
}

// 8/16/16 wiring, mix 16+16
void set_level_led34a_blend(uint8_t level) {
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    uint8_t blend = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, DSM_TOP, blend);

    set_hw_levels(0, warm_PWM,    cool_PWM,
                  0, (blend<170), (blend>85));
}

// 16/16/8 wiring, mix 16+8
void set_level_led34b_blend(uint8_t level) {
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    uint8_t blend = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, DSM_TOP, blend);

    set_hw_levels(cool_PWM,   warm_PWM,    0,
                  (blend>85), (blend<170), 0);
}

void set_level_hsv(uint8_t level) {
    RGB_t color;
    uint8_t h = cfg.channel_mode_args[channel_mode];
    uint8_t s = 255;  // TODO: drop saturation at brightest levels
    PWM_DATATYPE v = PWM_GET(pwm1_levels, level);
    color = hsv2rgb(h, s, v);

    set_hw_levels(color.r, color.g, color.b,
                  0,       0,       0);
}

// calculate a 3-channel "auto tint" blend
// (like red -> warm white -> cool white)
// results are placed in *a, *b, and *c vars
// level : ramp level to convert into 3 channel levels
// (assumes ramp table is "pwm1_levels")
void calc_auto_3ch_blend(
    PWM_DATATYPE *a,  // red
    PWM_DATATYPE *b,  // warm
    PWM_DATATYPE *c,  // cool
    uint8_t level) {

    PWM_DATATYPE vpwm  = PWM_GET(pwm1_levels, level);

    // tint goes from 0 (red) to 127 (warm white) to 255 (cool white)
    uint8_t mytint;
    mytint = 255 * (uint16_t)(level+1) / RAMP_SIZE;

    uint8_t falling=0, rising=0;
    if (level < (RAMP_SIZE/2))
        falling = 255 - triangle_wave(mytint);
    else
        rising = 255 - triangle_wave(mytint);

    // TODO: make "a" drop to zero sooner, and "c" start ramping up later
    // red is high at 0, low at 255 (linear)
    *a = (((PWM_DATATYPE2)falling
         * (PWM_DATATYPE2)vpwm) + 127) / 255;
    // warm white is low at 0 and 255, high at 127 (linear triangle)
    *b = (((PWM_DATATYPE2)triangle_wave(mytint)
         * (PWM_DATATYPE2)vpwm)      ) / 255;
    // cool white is low at 0, high at 255 (linear)
    *c = (((PWM_DATATYPE2)rising
         * (PWM_DATATYPE2)vpwm) + 127) / 255;

}

// 3-channel "auto tint" channel mode
void set_level_auto3(uint8_t level) {
    PWM_DATATYPE a, b, c;
    calc_auto_3ch_blend(&a, &b, &c, level);

    set_hw_levels(c, b, a,
                  0, 0, (0 == level));
}

///// "gradual tick" functions for smooth thermal regulation /////
// (and other smooth adjustments)

bool gradual_adjust(PWM_DATATYPE main2, PWM_DATATYPE led3, PWM_DATATYPE led4) {
    // adjust multiple times based on current brightness
    // (so it adjusts faster/coarser when bright, slower/finer when dim)

    // higher shift = slower/finer adjustments
    const uint8_t shift = 9;  // ((255 << 7) >> 9) = 63 max
    uint8_t steps;

    steps = main2_dsm_lvl >> shift;
    for (uint8_t i=0; i<=steps; i++)
        GRADUAL_ADJUST_SIMPLE(main2, main2_dsm_lvl);

    steps = led3_dsm_lvl  >> shift;
    for (uint8_t i=0; i<=steps; i++)
        GRADUAL_ADJUST_SIMPLE(led3,  led3_dsm_lvl );

    steps = led4_dsm_lvl  >> shift;
    for (uint8_t i=0; i<=steps; i++)
        GRADUAL_ADJUST_SIMPLE(led4,  led4_dsm_lvl );

    if ((main2 == main2_dsm_lvl)
     && (led3  == led3_dsm_lvl )
     && (led4  == led4_dsm_lvl )) {
        return true;  // done
    }
    return false;  // not done yet
}

bool gradual_tick_main2(uint8_t gt) {
    PWM_DATATYPE main2 = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(main2, 0, 0);
}

bool gradual_tick_led3(uint8_t gt) {
    PWM_DATATYPE led3 = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(0, led3, 0);
}

bool gradual_tick_led4(uint8_t gt) {
    PWM_DATATYPE led4 = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(0, 0, led4);
}

bool gradual_tick_all(uint8_t gt) {
    PWM_DATATYPE pwm = PWM_GET(pwm1_levels, gt);
    return gradual_adjust(pwm, pwm, pwm);
}

// 8/16/16 wiring, mix 16+16
bool gradual_tick_led34a_blend(uint8_t gt) {
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, gt);
    uint8_t blend = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, DSM_TOP, blend);

    return gradual_adjust(0, warm_PWM, cool_PWM);
}

// 16/16/8 wiring, mix 16+8
bool gradual_tick_led34b_blend(uint8_t gt) {
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET(pwm1_levels, gt);
    uint8_t blend = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, DSM_TOP, blend);

    return gradual_adjust(cool_PWM, warm_PWM, 0);
}

bool gradual_tick_hsv(uint8_t gt) {
    // figure out what exact PWM levels we're aiming for
    RGB_t color;
    uint8_t h = cfg.channel_mode_args[channel_mode];
    uint8_t s = 255;  // TODO: drop saturation at brightest levels
    PWM_DATATYPE v = PWM_GET(pwm1_levels, gt);
    color = hsv2rgb(h, s, v);

    return gradual_adjust(color.r, color.g, color.b);
}

bool gradual_tick_auto3(uint8_t gt) {
    // figure out what exact PWM levels we're aiming for
    PWM_DATATYPE red, warm, cool;
    calc_auto_3ch_blend(&red, &warm, &cool, gt);
    return gradual_adjust(cool, warm, red);
}

///// Chaotic pendulum color animation mode /////

// State variables for coupled oscillator chaos
static int16_t chaos_theta1 = 0;      // hue position (scaled, wraps at 65536)
static int16_t chaos_omega1 = 200;    // hue angular velocity
static int16_t chaos_theta2 = 0;      // saturation oscillator
static int16_t chaos_omega2 = 150;    // saturation velocity

// Convert triangle_wave output (0-255) to signed (-128..127)
static inline int8_t signed_wave(uint8_t phase) {
    return (int8_t)(triangle_wave(phase) - 128);
}

// Frame counter for driving oscillation
static uint8_t chaos_frame = 0;

// Physics update for coupled chaotic pendulum
static void chaos_update(void) {
    // Get energy from user config (0-255)
    // Map to useful range: avoid too-slow (boring) and too-fast (just white)
    uint8_t energy = cfg.channel_mode_args[channel_mode];
    // scale: 6-12 (was 4-19) - narrower range for perceptually useful speeds
    int16_t scale = 6 + ((energy * 6) >> 8);  // 6 + 0..5 = 6..11

    // Use triangle wave as sinusoidal approximation
    int8_t wave1 = signed_wave((uint8_t)(chaos_theta1 >> 8));
    int8_t wave2 = signed_wave((uint8_t)(chaos_theta2 >> 8));
    int8_t wave_diff = signed_wave((uint8_t)((chaos_theta2 - chaos_theta1) >> 8));

    // Driving force: periodic kick to sustain oscillations
    // This creates a driven double pendulum which exhibits true chaos
    chaos_frame++;
    int8_t drive = signed_wave(chaos_frame * 3);  // slow driving frequency
    // drive amplitude: 40-56 (was 32-63) - narrower for better behavior
    int16_t drive_force = (drive * (int16_t)(40 + (energy >> 4))) >> 7;

    // Coupled pendulum acceleration with driving
    int16_t acc1 = ((-20 * wave1) >> 4) + ((16 * wave_diff) >> 4) + drive_force;
    int16_t acc2 = ((-16 * wave2) >> 4) - ((16 * wave_diff) >> 4);

    // Very light damping (prevents runaway but allows sustained motion)
    chaos_omega1 = chaos_omega1 - (chaos_omega1 >> 9) + acc1;
    chaos_omega2 = chaos_omega2 - (chaos_omega2 >> 9) + acc2;

    // Velocity limits
    if (chaos_omega1 > 3000) chaos_omega1 = 3000;
    if (chaos_omega1 < -3000) chaos_omega1 = -3000;
    if (chaos_omega2 > 2500) chaos_omega2 = 2500;
    if (chaos_omega2 < -2500) chaos_omega2 = -2500;

    // Position update (scaled by energy)
    chaos_theta1 += (chaos_omega1 * scale) >> 4;
    chaos_theta2 += (chaos_omega2 * scale) >> 4;
}

void set_level_chaos(uint8_t level) {
    if (0 == level) { set_level_zero(); return; }

    // Advance chaos physics
    chaos_update();

    // Map theta1 to hue (full 0-255 range)
    uint8_t hue = (uint8_t)(chaos_theta1 >> 8);

    // Map theta2 to saturation with sqrt-like expansion
    // This spends less time near white (center) and more at saturated colors
    int8_t sat_raw = (int8_t)(chaos_theta2 >> 9);  // -128 to +127
    // Sqrt-like expansion: small values get pushed away from zero
    // Use (sign * sqrt(abs)) approximation via: sign * (128 - (128-abs)^2/128)
    uint8_t sat_abs = sat_raw < 0 ? -sat_raw : sat_raw;
    uint8_t inv = 128 - sat_abs;
    uint8_t sat_expanded = 128 - ((inv * inv) >> 7);  // sqrt-ish curve
    int16_t sat = 140 + (sat_raw < 0 ? -sat_expanded : sat_expanded);
    if (sat < 40) sat = 40;    // allow more saturated colors
    if (sat > 240) sat = 240;

    // Brightness from ramp level
    PWM_DATATYPE val = PWM_GET(pwm1_levels, level);

    // Convert to RGB and output
    RGB_t color = hsv2rgb(hue, (uint8_t)sat, val);
    set_hw_levels(color.r, color.g, color.b, 0, 0, 0);
}

bool gradual_tick_chaos(uint8_t gt) {
    // Always advance chaos animation (this is how we get continuous animation)
    chaos_update();

    // Compute target color
    uint8_t hue = (uint8_t)(chaos_theta1 >> 8);
    // Sqrt-like saturation expansion (same as set_level_chaos)
    int8_t sat_raw = (int8_t)(chaos_theta2 >> 9);
    uint8_t sat_abs = sat_raw < 0 ? -sat_raw : sat_raw;
    uint8_t inv = 128 - sat_abs;
    uint8_t sat_expanded = 128 - ((inv * inv) >> 7);
    int16_t sat = 140 + (sat_raw < 0 ? -sat_expanded : sat_expanded);
    if (sat < 40) sat = 40;
    if (sat > 240) sat = 240;

    PWM_DATATYPE val = PWM_GET(pwm1_levels, gt);
    RGB_t color = hsv2rgb(hue, (uint8_t)sat, val);

    // Smooth transition toward target
    gradual_adjust(color.r, color.g, color.b);

    // Always return false to keep animation running continuously
    return false;
}

///// 3H handler for chaos mode energy adjustment /////
// Shows brightness proportional to energy during hold, blinks on wrap
void save_config();  // forward declaration
uint8_t chaos_3h(Event event, uint16_t arg) {
    static int8_t direction = 1;
    static uint8_t active = 0;
    static uint8_t saved_level = 0;
    uint8_t energy = cfg.channel_mode_args[channel_mode];

    // click, click, hold: adjust energy with visual feedback
    if (event == EV_click3_hold) {
        // reset at beginning of movement
        if (! arg) {
            active = 1;
            saved_level = actual_level;  // remember brightness to restore later
        }
        if (! active) return EVENT_NOT_HANDLED;

        uint8_t old_energy = energy;
        energy += direction;
        cfg.channel_mode_args[channel_mode] = energy;

        // detect wrap-around and blink
        if ((old_energy == 255 && energy == 0) || (old_energy == 0 && energy == 255)) {
            set_level(0);
            delay_4ms(8);  // brief off
        }

        // show brightness proportional to energy (min 1 to keep light on)
        uint8_t display_level = 1 + (energy * (uint16_t)(MAX_LEVEL - 1)) / 255;
        set_level(display_level);

        return EVENT_HANDLED;
    }

    // click, click, hold, release: restore brightness and save
    else if (event == EV_click3_hold_release) {
        active = 0;
        direction = -direction;  // reverse for next time
        save_config();
        set_level(saved_level);  // restore original brightness
        return EVENT_HANDLED;
    }

    return EVENT_NOT_HANDLED;
}

