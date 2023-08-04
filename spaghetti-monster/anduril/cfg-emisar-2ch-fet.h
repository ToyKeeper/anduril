// Emisar 2-channel generic (plus FET) config options for Anduril (based on Noctigon K9.3)
// Copyright (C) 2021-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define MODEL_NUMBER "0136"
#include "hwdef-emisar-2ch-fet.h"
#include "hank-cfg.h"
// ATTINY: 1634

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// the aux LEDs are front-facing, so turn them off while main LEDs are on
// it also has an independent LED in the button
#define USE_BUTTON_LED
// TODO: the whole "indicator LED" thing needs to be refactored into
//       "aux LED(s)" and "button LED(s)" since they work a bit differently
// enabling this option breaks the button LED on D4v2.5
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

// channel modes...
// CM_CH1, CM_CH2, CM_BOTH, CM_BLEND, CM_AUTO
// enable max brightness out of the box
#define DEFAULT_CHANNEL_MODE           CM_BLEND

#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_BLEND

#define POLICE_COLOR_STROBE_CH1        CM_CH1
#define POLICE_COLOR_STROBE_CH2        CM_CH2

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
#define TINT_RAMPING_CORRECTION 0  // none, linear regulator doesn't need it


// channel 1 / main LEDs (w/ DD FET)
//   output: unknown, 1750 lm?
//   FET: unknown, 3000 lm?
// channel 2
//   output: unknown, 1750 lm?
// combined: 4000 to 5000 lm?
#define RAMP_SIZE 150

// linear+FET ramp: maxreg at 130/150
// level_calc.py 4.01 2 150 7135 1 0.1 1764 FET 1 10 3000 --pwm dyn:68:4096:255:3 --clock 8:16:0
// linear segment
#define PWM1_LEVELS 1,1,1,1,1,1,1,1,2,2,2,2,2,3,3,3,4,4,4,4,5,5,5,6,6,6,7,7,7,8,8,8,8,9,9,9,9,10,10,10,10,10,11,11,11,11,12,12,12,12,13,13,13,14,14,15,15,16,17,17,18,19,20,21,22,24,25,26,27,29,30,32,33,35,36,38,40,41,43,45,47,49,51,53,56,58,60,63,65,68,71,73,76,79,82,85,88,92,95,99,102,106,110,113,117,121,126,130,134,139,143,148,153,158,163,168,174,179,185,190,196,202,208,215,221,227,234,241,248,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
// DD FET segment
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,20,31,42,54,65,77,89,102,114,127,140,154,167,181,195,210,225,240,255
// PWM TOPS values
#define PWM3_LEVELS 4096,3768,3433,3094,2751,2405,2058,1713,2816,2415,2023,1642,1275,1877,1844,1564,1960,1725,1504,1298,1527,1352,1190,1324,1185,1058,1138,1029,928,977,890,810,738,769,706,648,595,615,569,526,486,450,464,432,402,374,384,359,336,314,321,301,283,288,271,275,259,262,263,249,250,251,251,250,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255

// linear-only ramp
// level_calc.py 3.11 1 150 7135 1 0.1 1750 --pwm dyn:64:4096:255:3 --clock 8:16:0
#define PWM4_LEVELS 1,1,1,1,1,1,2,2,2,3,3,3,4,4,4,5,5,6,6,6,7,7,7,8,8,8,9,9,9,10,10,10,10,10,10,11,11,11,11,11,11,11,12,12,12,12,12,12,13,13,13,14,14,14,15,15,16,17,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,33,34,35,37,38,39,41,42,44,45,47,48,50,52,53,55,57,59,61,63,65,67,69,71,73,75,77,79,82,84,86,89,91,94,97,99,102,105,107,110,113,116,119,122,125,128,132,135,138,141,145,148,152,156,159,163,167,170,174,178,182,186,190,195,199,203,208,212,217,221,226,230,235,240,245,250,255
// PWM_TOPS values for linear-only ramp
#define PWM5_LEVELS 4096,3681,3247,2794,2328,1856,2937,2393,1860,2690,2273,1875,2281,1959,1658,1893,1646,1774,1569,1381,1466,1309,1166,1224,1104,996,1033,942,858,882,810,746,687,634,586,604,561,522,487,454,425,397,409,385,362,341,321,302,311,295,279,286,271,257,263,250,255,258,246,249,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255

#define DEFAULT_LEVEL      70
#define MAX_1x7135         130
#define MAX_Nx7135         MAX_1x7135
#define HALFSPEED_LEVEL    16
#define QUARTERSPEED_LEVEL 8

#define RAMP_SMOOTH_FLOOR  10  // level 1 is unreliable (?)
#define RAMP_SMOOTH_CEIL   130
// 10, 30, 50, [70], 90, 110, [130]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit highest regulated power (no FET or turbo)
// 10, 40, [70], 100, 130
#define SIMPLE_UI_FLOOR  RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL   RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS  5

// stop panicking at ~1500 lm
#define THERM_FASTER_LEVEL 120
#define MIN_THERM_STEPDOWN 65  // should be above highest dyn_pwm level

#define USE_POLICE_COLOR_STROBE_MODE
#undef  TACTICAL_LEVELS
#define TACTICAL_LEVELS 130,30,(RAMP_SIZE+3)  // high, low, police strobe

// use the brightest setting for strobe
#define STROBE_BRIGHTNESS MAX_LEVEL
// slow down party strobe; this driver can't pulse for 1ms or less
#define PARTY_STROBE_ONTIME 2
// TODO: change speed per channel mode
// (the FET is really fast, but the regulator is not)
//#undef PARTY_STROBE_ONTIME

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 33

// the power regulator is a bit slow, so push it harder for a quick response from off
#define DEFAULT_JUMP_START_LEVEL 21
#define BLINK_BRIGHTNESS 40
#define BLINK_ONCE_TIME 12  // longer blink, since main LEDs are slow

#define THERM_CAL_OFFSET 5

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

