// BLF Lantern config options for Anduril
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define MODEL_NUMBER "0621"
#include "hwdef-blf-lt1.h"
// ATTINY: 85

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING
// off mode: low (1)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)

// channel modes...
// CM_CH1, CM_CH2, CM_BOTH, CM_BLEND, CM_AUTO
#define DEFAULT_CHANNEL_MODE  CM_AUTO

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
//#define TINT_RAMPING_CORRECTION 26  // prototype, 140%
//#define TINT_RAMPING_CORRECTION 10  // production model, 115%
#define TINT_RAMPING_CORRECTION 0  // none

#define RAMP_SIZE 150
// delta-sigma modulated PWM (0b0HHHHHHHHLLLLLLL = 0, 8xHigh, 7xLow bits)
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
// level_calc.py 5.01 1 150 7135 2 0.2 600 --pwm 32640
//#define PWM1_LEVELS  2,4,5,7,9,12,14,17,20,23,27,31,35,39,44,50,56,62,68,76,83,91,100,110,120,130,142,154,167,181,195,211,227,244,263,282,303,324,347,371,397,424,452,482,513,545,580,616,653,693,734,778,823,871,920,972,1026,1083,1142,1203,1267,1334,1403,1475,1551,1629,1710,1795,1883,1974,2069,2167,2269,2375,2485,2599,2717,2839,2965,3096,3232,3372,3517,3667,3822,3982,4148,4319,4495,4677,4865,5060,5260,5466,5679,5899,6125,6358,6599,6846,7101,7363,7633,7911,8197,8491,8793,9104,9424,9753,10090,10437,10794,11160,11536,11922,12319,12726,13143,13572,14011,14462,14925,15399,15885,16383,16894,17417,17954,18503,19066,19642,20232,20837,21456,22089,22737,23400,24079,24774,25484,26211,26954,27713,28490,29284,30096,30926,31774,32640
// level_calc.py 5.01 1 150 7135 128 0.2 600 --pwm 32640
#define PWM1_LEVELS  128,130,131,133,135,138,140,143,146,149,153,157,161,165,170,176,181,188,194,201,209,217,226,235,245,256,267,279,292,306,320,336,352,369,388,407,428,449,472,496,521,548,576,606,637,669,703,739,777,816,858,901,946,993,1043,1094,1148,1204,1263,1324,1388,1455,1524,1596,1671,1749,1830,1914,2002,2093,2187,2285,2387,2492,2601,2715,2832,2954,3080,3210,3345,3485,3629,3779,3933,4093,4258,4428,4604,4785,4973,5166,5366,5571,5783,6002,6228,6460,6699,6946,7199,7461,7730,8006,8291,8584,8885,9195,9514,9841,10178,10523,10878,11243,11618,12002,12397,12803,13219,13645,14083,14532,14993,15465,15949,16446,16955,17476,18010,18558,19118,19692,20280,20882,21499,22130,22775,23436,24112,24804,25512,26235,26976,27732,28506,29297,30106,30932,31777,32640

#define DEFAULT_LEVEL 75
#define MAX_1x7135 75
#define HALFSPEED_LEVEL 44
#define QUARTERSPEED_LEVEL 34
#undef USE_DYNAMIC_UNDERCLOCKING  // makes huge bumps in the ramp

#define USE_SMOOTH_STEPS
//#define USE_SET_LEVEL_GRADUALLY

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 40

// override default ramp style
#undef RAMP_STYLE
#define RAMP_STYLE  1  // 0 = smooth, 1 = stepped
// set floor and ceiling as far apart as possible
// because this lantern isn't overpowered
#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     150
#define RAMP_DISCRETE_FLOOR  1
#define RAMP_DISCRETE_CEIL   150
#define RAMP_DISCRETE_STEPS  7

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE
#define USE_EXTENDED_SIMPLE_UI

// LT1 can handle heat well, so don't limit simple mode
#define SIMPLE_UI_FLOOR  10
#define SIMPLE_UI_CEIL   150
#define SIMPLE_UI_STEPS  5

// also at Sofirn's request, enable 2 click turbo (Anduril 1 style)
#define DEFAULT_2C_STYLE  1


// the sensor (attiny85) is nowhere near the emitters
// so thermal regulation can't work
#ifdef USE_THERMAL_REGULATION
#undef USE_THERMAL_REGULATION
#endif

// don't blink while ramping
#ifdef BLINK_AT_RAMP_FLOOR
#undef BLINK_AT_RAMP_FLOOR
#endif
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif
#ifdef BLINK_AT_RAMP_CEIL
#undef BLINK_AT_RAMP_CEIL
#endif

// too big, turn off extra features
//#undef USE_STEPPED_TINT_RAMPING
#undef USE_MOMENTARY_MODE
#undef USE_TACTICAL_MODE
#undef USE_SOS_MODE
//#undef USE_SIMPLE_UI
//#undef USE_BEACON_MODE
//#undef USE_RAMP_SPEED_CONFIG
#undef USE_RAMP_AFTER_MOON_CONFIG
#undef USE_2C_STYLE_CONFIG
#undef USE_VOLTAGE_CORRECTION
//#undef USE_CHANNEL_PER_STROBE
// party strobe, tac strobe, lightning, candle, bike
#define DEFAULT_STROBE_CHANNELS  CM_BOTH,CM_BOTH,CM_AUTO,CM_AUTO,CM_AUTO

// for consistency with other models
#define USE_SOFT_FACTORY_RESET

