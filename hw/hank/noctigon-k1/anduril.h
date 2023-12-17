// Noctigon K1 config options for Anduril
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// (originally known as Emisar D1S v2)
#include "hank/noctigon-k1/hwdef.h"
#include "hank/anduril.h"

// this light can safely run a bit hotter than most
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 55

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
#define USE_AUX_RGB_LEDS_WHILE_ON  5
#define USE_CONFIGURABLE_RGB_VOLTAGE_LEVELS
#define USE_INDICATOR_LED_WHILE_RAMPING


#define RAMP_SIZE 150

// ../../bin/level_calc.py cube 1 150 7135 1 4 1300
// (with max_pwm set to 1023)
// (level 0 flickers and isn't relevant on a thrower, so it's omitted)
#define RAMP_LENGTH 150
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,6,6,7,8,9,10,11,12,13,14,15,16,17,18,20,21,23,24,26,27,29,31,32,34,36,38,40,43,45,47,49,52,54,57,60,62,65,68,71,74,77,81,84,87,91,95,98,102,106,110,114,118,122,127,131,136,141,145,150,155,160,166,171,176,182,188,193,199,205,211,218,224,231,237,244,251,258,265,272,280,287,295,303,310,319,327,335,344,352,361,370,379,388,397,407,416,426,436,446,457,467,477,488,499,510,521,533,544,556,568,580,592,604,617,629,642,655,668,682,695,709,723,737,751,766,781,795,810,826,841,857,872,888,904,921,937,954,971,988,1005,1023

#define MAX_1x7135 50
#define DEFAULT_LEVEL 50
#define MIN_THERM_STEPDOWN 50  // should be above highest dyn_pwm level
//#define HALFSPEED_LEVEL 12
//#define QUARTERSPEED_LEVEL 4
// don't slow down at low levels; this isn't that sort of light
// (it needs to stay at full speed for the 10-bit PWM to work)
#ifdef USE_DYNAMIC_UNDERCLOCKING
#undef USE_DYNAMIC_UNDERCLOCKING
#endif

#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  130
// 10, 30, [50], 70, 90, 110, 130
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit ~75% power
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS 5

// stop panicking at ~70% power or ~600 lm
#define THERM_FASTER_LEVEL 130

#define THERM_CAL_OFFSET 5

#define THERM_RESPONSE_MAGNITUDE 32  // smaller adjustments, this host changes temperature slowly
#define THERM_NEXT_WARNING_THRESHOLD 32  // more error tolerance before adjusting

// the power regulator is a bit slow, so push it harder for a quick response from off
#define DEFAULT_JUMP_START_LEVEL 10
#define BLINK_BRIGHTNESS DEFAULT_LEVEL
#define BLINK_ONCE_TIME 12

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// there is usually no lighted button,
// so blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN

// slow down party strobe; this driver can't pulse for 1ms or less
#define PARTY_STROBE_ONTIME 2

// make candle mode wobble more
#define CANDLE_AMPLITUDE 32

// don't blink while ramping; the entire ramp is regulated
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

