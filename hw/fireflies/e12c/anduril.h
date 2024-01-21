// FireflyLite E12C (FET+11+1) driver layout
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fireflies/e12c/hwdef.h"

//***************************************
//**    RAMP TABLE AND OPERATIONS      **
//***************************************

#define RAMP_SIZE 150

// level_calc.py seventh 3 150 7135 1 1 150 7135 6 1 1800 FET 3 10 10000
#define PWM1_LEVELS 1,1,2,2,2,3,3,4,4,5,5,6,7,7,8,9,10,11,12,14,15,17,18,20,22,24,26,28,31,33,36,39,43,46,50,54,59,63,68,74,79,85,92,99,106,114,122,131,140,150,161,172,183,196,209,223,237,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,9,10,12,13,15,17,19,21,23,25,27,30,32,35,38,41,44,47,51,54,58,62,66,70,74,79,84,89,94,100,105,111,118,124,131,138,145,153,161,169,178,187,196,206,216,226,237,248,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM3_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,7,10,12,16,19,22,25,29,33,36,40,44,49,53,57,62,67,72,77,82,88,94,100,106,112,118,125,132,139,146,154,162,170,178,187,196,205,214,224,234,244,255

#define DEFAULT_LEVEL       58
#define MAX_1x7135          58
#define MAX_Nx7135          107
#define HALFSPEED_LEVEL     15
#define QUARTERSPEED_LEVEL  6

// go up to ~3000 lm
#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  118
#define RAMP_DISCRETE_FLOOR 1
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 6

// safe limit ~20% power / max regulated
// 20 40 60 80 100
#define SIMPLE_UI_FLOOR  5
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
//#define USE_BEACONTOWER_MODE

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
//#undef USE_SUNSET_TIMER // uses 484 bytes!!
#undef USE_AUX_RGB_LEDS
#undef USE_MOMENTARY_MODE
#undef USE_SOS_MODE
