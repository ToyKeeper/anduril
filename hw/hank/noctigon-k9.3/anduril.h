// Noctigon K9.3 config options for Anduril
// Copyright (C) 2020-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "hank/emisar-2ch/fet/hwdef.h"
#include "hank/anduril.h"

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
#define DEFAULT_CHANNEL_MODE           CM_BOTH

#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_BOTH

#define POLICE_COLOR_STROBE_CH1        CM_CH1
#define POLICE_COLOR_STROBE_CH2        CM_CH2

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
#define TINT_RAMPING_CORRECTION 0  // none, linear regulator doesn't need it


// main LEDs
//   max regulated: 1500 to 2000 lm?
//   FET: 5000 to 8000 lm?
// 2nd LEDs
//   max regulated: ~1500 lm
#define RAMP_SIZE 150

// linear+FET ramp: maxreg at 120/150
// level_calc.py 5.01 2 150 7135 1 0.1 1872 FET 1 10 5000 --pwm dyn:63:4096:255:3 --clock 8:16:0
// linear segment
#define PWM1_LEVELS 1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,5,6,6,6,6,6,6,7,7,7,7,7,8,8,8,8,9,9,10,10,10,11,12,12,13,14,14,16,17,18,19,20,21,22,24,25,26,28,29,31,33,35,36,38,40,42,45,47,49,52,54,57,60,63,66,69,72,76,79,83,87,91,95,99,103,108,112,117,122,127,133,138,144,150,156,162,169,175,182,189,197,204,212,220,229,237,246,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
// DD FET segment
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,11,17,24,30,36,43,50,57,64,71,79,87,95,103,111,120,129,138,147,157,167,177,187,198,209,220,231,243,255
// PWM TOPS values
#define PWM3_LEVELS 4096,3798,3504,3214,2926,2645,2368,2098,1834,1579,1333,2304,2017,1744,1486,1242,1174,1700,1505,1323,1155,1037,1299,1163,1038,923,819,967,874,789,712,641,577,655,598,545,497,453,413,456,419,386,355,326,352,326,301,278,296,275,287,267,249,258,265,248,253,256,240,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255

// linear-only ramp
// level_calc.py 3.11 1 150 7135 1 0.1 1750 --pwm dyn:64:4096:255:3 --clock 8:16:0
#define PWM4_LEVELS 1,1,1,1,1,1,2,2,2,3,3,3,4,4,4,5,5,6,6,6,7,7,7,8,8,8,9,9,9,10,10,10,10,10,10,11,11,11,11,11,11,11,12,12,12,12,12,12,13,13,13,14,14,14,15,15,16,17,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,33,34,35,37,38,39,41,42,44,45,47,48,50,52,53,55,57,59,61,63,65,67,69,71,73,75,77,79,82,84,86,89,91,94,97,99,102,105,107,110,113,116,119,122,125,128,132,135,138,141,145,148,152,156,159,163,167,170,174,178,182,186,190,195,199,203,208,212,217,221,226,230,235,240,245,250,255
// PWM_TOPS values for linear-only ramp
#define PWM5_LEVELS 4096,3681,3247,2794,2328,1856,2937,2393,1860,2690,2273,1875,2281,1959,1658,1893,1646,1774,1569,1381,1466,1309,1166,1224,1104,996,1033,942,858,882,810,746,687,634,586,604,561,522,487,454,425,397,409,385,362,341,321,302,311,295,279,286,271,257,263,250,255,258,246,249,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255

#define DEFAULT_LEVEL      65
#define MAX_1x7135         120
#define MAX_Nx7135         MAX_1x7135
#define HALFSPEED_LEVEL    16
#define QUARTERSPEED_LEVEL 8

#define RAMP_SMOOTH_FLOOR  10  // level 1 is unreliable (?)
#define RAMP_SMOOTH_CEIL   120
// 10, 28, 46, [65], 83, 101, 120
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// 10, 37, [65], 92, 120
#define SIMPLE_UI_FLOOR  RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL   RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS  5

// stop panicking at ~25% power or ~1000 lm
#define THERM_FASTER_LEVEL 100
#define MIN_THERM_STEPDOWN 60  // should be above highest dyn_pwm level

#define USE_POLICE_COLOR_STROBE_MODE
#undef  TACTICAL_LEVELS
#define TACTICAL_LEVELS 120,30,(RAMP_SIZE+3)  // high, low, police strobe

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
#define DEFAULT_JUMP_START_LEVEL 35
#define BLINK_BRIGHTNESS 30
#define BLINK_ONCE_TIME 12  // longer blink, since main LEDs are slow

#define THERM_CAL_OFFSET 5

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

#define USE_PREVIOUS_CHANNEL
