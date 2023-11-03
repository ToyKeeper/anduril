// gChart's custom FET+1 driver config options for Anduril
// Copyright (C) 2020-2023 gchart, Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "gchart/fet1-t1616/hwdef.h"

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING
// off mode: low (1)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)

#define RAMP_SIZE 150

// 7135 at 90/150
// level_calc.py 5.7895 2 150 7135 0 0.1 125.25 FET 1 10 1200 --pwm dyn:61:4096:255:2.5 --clock 5:11:2.0
// (with heavy manual tweaks up to ~15/150)
#define PWM1_LEVELS     1,   1,   2,   2,   3,   3,   4,   5,   6,   6,   8,   9,   9,  10,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  19,  20,  21,  22,  23, 23, 24, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 35, 36, 37, 38, 40, 41, 43, 45, 47, 50, 53, 56, 60, 63, 67, 71, 75, 79, 84, 89, 94, 99,104,110,116,122,129,136,143,150,158,166,174,183,192,202,211,222,232,243,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  3,  5,  7,  8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 29, 31, 34, 37, 39, 42, 45, 48, 51, 54, 58, 61, 65, 68, 72, 76, 80, 84, 88, 93, 97,102,107,112,117,122,127,133,139,145,151,157,163,170,177,183,191,198,205,213,221,229,238,246,255
#define PWM_TOPS     4095,2893,3917,2806,3252,2703,2684,2660,2640,2370,3000,2900,2630,2549,2246,2193,2030,1961,1889,1716,1642,1569,1497,1428,1290,1232,1176,1122,1070,976,932,890,849,779,745,685,656,605,579,536,514,476,457,424,407,379,364,340,327,314,302,291,280,276,266,262,257,253,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
#define MAX_1x7135 90
#define MIN_THERM_STEPDOWN 60
#define HALFSPEED_LEVEL 11
#define QUARTERSPEED_LEVEL 5
#define DEFAULT_LEVEL 50

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     130
// 10 30 50 70 [90] 110 130
#define RAMP_DISCRETE_FLOOR  10
#define RAMP_DISCRETE_CEIL   130
#define RAMP_DISCRETE_STEPS  7

// 10 30 50 70 [90] 110 130
#define SIMPLE_UI_FLOOR  10
#define SIMPLE_UI_CEIL   130
#define SIMPLE_UI_STEPS  7

// stop panicking at ~50% power
#define THERM_FASTER_LEVEL 130  // throttle back faster when high

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the aux LEDs by default
//#define DEFAULT_BLINK_CHANNEL  CM_AUX

// the default of 26 looks a bit rough, so increase it to make it smoother
#define CANDLE_AMPLITUDE 40

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// enable factory reset on 13H without loosening tailcap (required)
#define USE_SOFT_FACTORY_RESET

