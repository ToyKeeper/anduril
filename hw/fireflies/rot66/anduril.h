// Fireflies ROT66 (14x7135) config options for Anduril
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fireflies/rot66/hwdef.h"

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING

// off mode: high (2)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 2)

#define RAMP_SIZE 150

// driver is a FET+N+1,
// where N=6 for the 219b version,
// or N=13 for the XP-L HI version (this version)
// calculated to get transition points at level 65 and 125
// first 65 steps copied from FW3A
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,6,7,8,9,10,12,13,14,15,17,19,20,22,24,26,29,31,34,36,39,42,45,48,51,55,59,62,66,70,75,79,84,89,93,99,104,110,115,121,127,134,140,147,154,161,168,176,184,192,200,209,217,226,236,245,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
// remaining steps from this command:
// level_calc.py ninth 3 150 7135 1 1.4 110.1 7135 15 1 1644.16 FET 1 10 4500
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,18,19,20,21,22,23,25,26,27,29,30,32,34,35,37,39,41,43,46,48,50,53,55,58,61,64,67,70,73,77,81,84,88,92,97,101,106,110,115,120,126,131,137,143,149,156,162,169,177,184,192,200,208,217,226,235,245,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM3_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,21,28,35,43,51,60,68,77,87,96,106,117,127,138,150,161,173,186,199,212,226,240,255
#define MAX_1x7135          65
#define MAX_Nx7135          125
#define HALFSPEED_LEVEL     14
#define QUARTERSPEED_LEVEL  8

#define RAMP_SMOOTH_FLOOR  1
#define RAMP_SMOOTH_CEIL   MAX_Nx7135
// safe limit max regulated power
// 20 46 72 98 [125]
#define SIMPLE_UI_FLOOR  RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL   MAX_Nx7135
#define SIMPLE_UI_STEPS  5

// regulate down faster when the FET is active, slower otherwise
#define THERM_FASTER_LEVEL 130  // throttle back faster when high

// don't do this
#undef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_CEIL

// enable extra features
#define USE_SMOOTH_STEPS

// too big, turn off extra features
#undef USE_MOMENTARY_MODE
#undef USE_TACTICAL_MODE
#undef USE_SOS_MODE
//#undef USE_RAMP_AFTER_MOON_CONFIG
//#undef USE_RAMP_SPEED_CONFIG
//#undef USE_VOLTAGE_CORRECTION
//#undef USE_2C_STYLE_CONFIG
//#undef USE_TACTICAL_STROBE_MODE
//#ifdef USE_LOWPASS_WHILE_ASLEEP
//#undef USE_LOWPASS_WHILE_ASLEEP
//#endif

