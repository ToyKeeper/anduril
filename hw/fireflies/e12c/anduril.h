// FireflyLite E12C (FET+11+1) driver layout
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fireflies/e12c/hwdef.h"

//***************************************
//**    RAMP TABLE AND OPERATIONS      **
//***************************************

#define RAMP_SIZE 150

// level_calc.py fifth 3 150 7135 1 1 150 7135 6 1 1800 FET 3 10 10000
#define PWM1_LEVELS 1,2,2,2,3,3,4,5,6,7,8,9,10,11,13,14,16,18,20,23,25,28,31,34,37,41,45,49,54,59,64,70,76,83,90,97,105,114,123,132,143,153,165,177,190,203,218,233,249,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,9,10,12,13,15,17,19,21,23,25,28,30,33,35,38,41,44,47,50,54,57,61,65,69,73,78,82,87,92,97,102,108,114,120,126,132,139,146,153,160,168,175,184,192,201,210,219,228,238,249,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM3_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,6,9,11,14,17,20,23,26,29,32,35,39,42,46,50,53,57,61,66,70,74,79,83,88,93,98,103,108,114,119,125,131,137,143,149,156,162,169,176,183,190,198,205,213,221,229,238,246,255

#define DEFAULT_LEVEL       49
#define MAX_1x7135          49
#define MAX_Nx7135          100
#define HALFSPEED_LEVEL     15
#define QUARTERSPEED_LEVEL  6

// go up to ~3000 lm
#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  113
#define RAMP_DISCRETE_FLOOR 1
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 6

// safe limit ~20% power / max regulated
// 20 40 60 80 100
#define SIMPLE_UI_FLOOR  2
#define SIMPLE_UI_CEIL   MAX_Nx7135
#define SIMPLE_UI_STEPS  5

// set this light to use stepped ramp by default (instead of smooth)
#undef RAMP_STYLE
#define RAMP_STYLE 1

// enable extra features
#define USE_SMOOTH_STEPS

//***************************************
//**          USER INTERFACE           **
//***************************************

// only blink at max regulated level and ceiling
//#define BLINK_AT_RAMP_MIDDLE
//#define BLINK_AT_RAMP_MIDDLE_1 MAX_Nx7135
#define BLINK_AT_RAMP_CEIL
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// Allow 3C (or 6C) in Simple UI (toggle smooth or stepped ramping)
#define USE_SIMPLE_UI_RAMPING_TOGGLE

// allow Aux Config and Strobe Modes in Simple UI
#define USE_EXTENDED_SIMPLE_UI

// double click while on goes to full-power turbo, not ramp ceiling
#define DEFAULT_2C_STYLE 1

//***************************************
//**       THERMAL SETTINGS            **
//***************************************

// stop panicking at about ~4000lm
#define THERM_FASTER_LEVEL 120

#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 50

//***************************************
//**       AUX LEDs and MISC           **
//***************************************

// the button lights up (uses 294 bytes)
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING
// off mode: high (2)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 2)

// enable beacontower mode (uses 186 bytes)
#define USE_BEACONTOWER_MODE

// code too big for attiny85.. remove stuff to make room
// total compiled 8182 bytes (99.9% full..)
//#undef USE_SOS_MODE
//#undef USE_RAMP_AFTER_MOON_CONFIG
//#undef USE_RAMP_SPEED_CONFIG
//#undef USE_VOLTAGE_CORRECTION
//#undef USE_2C_STYLE_CONFIG
//#undef USE_TACTICAL_STROBE_MODE
//#define USE_SOFT_FACTORY_RESET
#undef USE_TACTICAL_MODE
#undef USE_SUNSET_TIMER // uses 484 bytes!!
#undef USE_AUX_RGB_LEDS
