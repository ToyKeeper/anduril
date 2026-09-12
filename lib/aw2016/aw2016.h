// aw2016.h: Device driver for AW2016 peripheral
// Copyright (C) 2025 - 2026 Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <stdint.h>
#include <avr/pgmspace.h>  // for PROGMEM


////////// defs and types //////////

// if you #include this file, assume you're using this chip and this code
#define USE_AW2016

// rgb channel numbers (different per build target)
#define AW_R_CH  1
#define AW_G_CH  0
#define AW_B_CH  2

#define RGB_BITS  8

// available aux RGB hardware power levels
// (power used at PWM=255)
typedef enum {
    AW2016_5mA  = 0b10,
    AW2016_10mA = 0b11,
    AW2016_15mA = 0b00,
    AW2016_30mA = 0b01,
} aw2016_current_t;


////////// const data //////////

// coarse brightness ramp and color balance using as little PWM as possible
// PWM modes are hella strobey, but it's unavoidable
// (power levels are estimated, not actual, and don't include overhead)
// FIXME: green is still too bright sometimes
PROGMEM const uint8_t aw2016_ramp[] = {
    // power   ,  R, G, B, PWM
    AW2016_5mA ,  4, 1, 1,   2,  // barely visible
    AW2016_5mA ,  4, 1, 1,   6,  // 0.03 mA
    AW2016_5mA ,  4, 1, 1,  24,  // 0.13 mA  <-- about right for "aux low" mode
    AW2016_5mA ,  4, 1, 1,  75,  // 0.4 mA
    AW2016_5mA ,  4, 1, 1, 150,  // 0.8 mA
    AW2016_5mA ,  4, 1, 1, 255,  // 1.3 mA
    AW2016_5mA ,  9, 2, 3, 255,  //   3 mA  <-- default POVD
    AW2016_5mA , 12, 3, 4, 255,  //   4 mA
    AW2016_5mA , 15, 4, 5, 255,  //   5 mA  <-- about right for "aux high" mode
    AW2016_10mA, 12, 3, 4, 255,  //   8 mA
    AW2016_10mA, 15, 4, 5, 255,  //  10 mA
    AW2016_15mA, 15, 4, 5, 255,  //  15 mA
    AW2016_30mA, 12, 3, 4, 255,  //  24 mA
    AW2016_30mA, 15, 4, 5, 255,  //  30 mA  <-- "On" or "R G Bling" modes
};
#define aw2016_ramp_get(row,col)  pgm_read_byte(aw2016_ramp + (5*row) + col)
#define AW2016_RAMP_SIZE    (sizeof(aw2016_ramp))
#define AW2016_RAMP_LEVELS  (sizeof(aw2016_ramp)/5)
#define AW2016_RAMP_DEFAULT    8
#define AW2016_RAMP_MIN_NOPWM  5
// default for each configurable brightness
// (0-based row indexes into table above)
#define AW2016_DEFAULT_LO    2
#define AW2016_DEFAULT_HI    8
#define AW2016_DEFAULT_POVD  6
#define AW2016_DEFAULT_ON    (AW2016_RAMP_LEVELS-1)  // brightest level

//typedef enum {
//    AW2016_MODE_PASSIVE,
//    AW2016_MODE_ACTIVE,
//    AW2016_MODE_POVD,
//    AW2016_MODE_ANIM,
//} aw2016_mode_t;


////////// globals //////////

uint8_t aw2016_is_awake = 0;
uint8_t aw2016_is_pwm_mode = 0;
uint8_t aw2016_animation_running = 0;
uint8_t aw2016_ramp_row = 0;
// allow sleep by default, set to 1 if fast response required from driver
uint8_t aw2016_do_not_sleep = 0;
// most recent RGB values set in PWM mode
RGB8_t aw2016_last_pwm;


////////// public API functions //////////

// call once at boot, from hwdef_setup()
inline void aw2016_hwdef_setup ();
// reset to default state
void aw2016_init ();

// activate a row in the aw2016 brightness ramp
void aw2016_set_ramp_current (uint8_t row);
// helper for rotating through ramp levels
uint8_t aw2016_next_ramp_row (uint8_t row, bool nopwm);

// pre-built effects
void aw2016_rainbow_rgb ();
void aw2016_rainbow_blend ();


////////// common auxrgb API stuff //////////

// emulate passive auxrgb
#undef set_auxrgb_power
#define set_auxrgb_power  aw2016_set_auxrgb_power
void aw2016_set_auxrgb_power (uint8_t power);

// change to active PWM mode or "passive" off/low/high mode
#define enable_auxrgb_pwm  aw2016_enable_auxrgb_pwm
#define disable_auxrgb_pwm  aw2016_disable_auxrgb_pwm
void aw2016_enable_auxrgb_pwm ();
void aw2016_disable_auxrgb_pwm ();

// set RGB values for active PWM mode
#define set_auxrgb_pwm  aw2016_set_auxrgb_pwm
void aw2016_set_auxrgb_pwm (RGB8_t color);

// smooth changes between pwm ramp levels
#define gradual_adjust_auxrgb  aw2016_gradual_adjust_auxrgb_pwm
bool aw2016_gradual_adjust_auxrgb_pwm (RGB8_t target);

// set the user's configured power / brightness level for RGB modes
#define hwdef_set_channel_mode  aw2016_set_channel_mode
void aw2016_set_channel_mode (uint8_t mode);

