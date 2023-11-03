// Mateminco/Astrolux MF01-Mini options for Anduril
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "mateminco/mf01-mini/hwdef.h"

// the button lights up
#define USE_INDICATOR_LED
//#define INDICATOR_LED_SKIP_LOW  // low mode doesn't work on this driver
// the button is visible while main LEDs are on
//#define USE_INDICATOR_LED_WHILE_RAMPING
// off mode: low (1)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)


#define RAMP_SIZE 150

// measured brightness with Sofirn 5500mAh cell at 3.97V:
// moon: 0.3 lm
// channel 1: 113 lm
// channel 2: 718 lm
// channel 3: 3500 lm
// ../../../bin/level_calc.py ninth 3 150 7135 1 2.5 115.65 7135 11 5 708.65 FET 1 10 3500
// (plus some manual tweaks for a smoother ramp)
#define PWM1_LEVELS 1,1,2,2,3,3,4,5,5,6,7,8,9,9,10,14,15,16,17,18,19,20,21,22,24,26,28,30,32,34,37,39,42,45,48,51,54,58,62,65,69,74,78,83,88,93,98,104,110,116,123,130,137,145,153,161,170,179,188,198,208,219,231,243,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,13,16,18,20,23,25,28,31,34,37,40,43,47,50,54,58,62,66,70,75,80,85,90,95,100,106,112,118,125,131,138,145,153,161,169,177,185,194,204,213,223,233,244,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM3_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,6,10,13,17,21,24,28,33,37,41,46,50,55,60,66,71,76,82,88,94,101,107,114,121,128,135,143,151,159,167,176,185,194,203,213,223,233,244,255

#define MAX_1x7135         65  // ~113 lm
#define MAX_Nx7135         110
#define HALFSPEED_LEVEL    16
#define QUARTERSPEED_LEVEL 8

#define RAMP_SMOOTH_FLOOR 1   // ~0.3 lm
#define RAMP_SMOOTH_CEIL  130  // ~??? lm
// 14/135/6 = 14, 38, 62, 86, [110], 135
// 20/110/7 = 20, 35, 50, [65], 80, 95, [110]
// 15/130/7 = 15, 34, 53, 72, 91, [110], 130  <--
#define RAMP_DISCRETE_FLOOR 15 // ~?? lm
#define RAMP_DISCRETE_CEIL  130 // ~??? lm
#define RAMP_DISCRETE_STEPS 7  // ??, ??, ... lm

// safe limit max regulated power
// 15 38 62 86 [110]
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL 110
#define SIMPLE_UI_STEPS 5


#define THERM_FASTER_LEVEL 130  // throttle back faster when high

// don't blink during ramp
//#define BLINK_AT_RAMP_CEIL
#undef BLINK_AT_RAMP_CEIL
#undef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_FLOOR

// enable extra features
#define USE_SMOOTH_STEPS

// too big, turn off extra features
#define USE_SOFT_FACTORY_RESET
//#undef USE_SIMPLE_UI
#undef USE_TACTICAL_MODE
#undef USE_SOS_MODE
#undef USE_BEACON_MODE
#undef USE_RAMP_AFTER_MOON_CONFIG
//#undef USE_RAMP_SPEED_CONFIG
#undef USE_VOLTAGE_CORRECTION
//#undef USE_2C_STYLE_CONFIG

