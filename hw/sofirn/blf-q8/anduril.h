// BLF Q8 config options for Anduril
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "sofirn/blf-q8/hwdef.h"

#define RAMP_SIZE 150

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING
// off mode: high (2)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 2)

// copied from Emisar D4 ramp
// ../../bin/level_calc.py 1 65 7135 1 0.8 150
// ... mixed with this:
// ../../bin/level_calc.py 2 150 7135 4 0.33 150 FET 1 10 1500
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,6,7,8,9,10,12,13,14,15,17,19,20,22,24,26,29,31,34,36,39,42,45,48,51,55,59,62,66,70,75,79,84,89,93,99,104,110,115,121,127,134,140,147,154,161,168,176,184,192,200,209,217,226,236,245,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,4,5,7,8,9,11,12,14,15,17,19,20,22,24,25,27,29,31,33,35,37,39,41,43,45,48,50,52,55,57,59,62,64,67,70,72,75,78,81,84,87,90,93,96,99,102,105,109,112,115,119,122,126,129,133,137,141,144,148,152,156,160,165,169,173,177,182,186,191,195,200,205,209,214,219,224,229,234,239,244,250,255

#define MAX_1x7135 65
#define DEFAULT_LEVEL 65
#define HALFSPEED_LEVEL 14
#define QUARTERSPEED_LEVEL 5

#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL 120
// 10 28 46 [65] 83 101 120
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// at Sofirn's request, use max (150) for the Simple UI ceiling
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL 150
#define SIMPLE_UI_STEPS 5

// also at Sofirn's request, enable 2 click turbo (Anduril 1 style)
#define DEFAULT_2C_STYLE 1

// enable SOS in the blinkies group
#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE
#define USE_EXTENDED_SIMPLE_UI

// stop panicking at ~75% power or ~3000 lm, this light has high thermal mass
#define THERM_FASTER_LEVEL (RAMP_SIZE*9/10)  // throttle back faster when high

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

#define USE_SMOOTH_STEPS

// too big, remove stuff to make room
#undef USE_SOS_MODE
//#undef USE_RAMP_AFTER_MOON_CONFIG
//#undef USE_RAMP_SPEED_CONFIG
//#undef USE_VOLTAGE_CORRECTION
//#undef USE_2C_STYLE_CONFIG
//#undef USE_TACTICAL_STROBE_MODE
#undef USE_TACTICAL_MODE
