// Sofirn SP10 Pro config options for Anduril
// Copyright (C) 2022-2023 (original author TBD), Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "sofirn/sp10-pro/hwdef.h"

// 1....15: level_calc.py 3.01 1  15 7135 1 0.1   2 --pwm dyn:15:64:64
// 16..150: level_calc.py 5.01 1 135 7135 1   2 800 --pwm dyn:49:3072:255:3.0
#define RAMP_SIZE 150
#define _PWM1_LEVELS_  1, 2, 4, 6, 9,12,15,19,23,28,34,41,48,55,64
#define _PWM1_TOPS_   64,64,64,64,64,64,64,64,64,64,64,64,64,64,64
#define _PWM2_LEVELS_    1,   1,   2,   2,   3,   3,   4,   4,   5,   5,   6,   6,   6,   7,   7,   7,   7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 33, 34, 35, 37, 38, 39, 41, 42, 44, 46, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 70, 72, 74, 77, 79, 82, 85, 88, 90, 93, 96, 99,103,106,109,113,116,120,123,127,131,135,139,143,147,151,156,160,165,170,175,180,185,190,195,201,206,212,218,223,230,236,242,248,255
#define _PWM2_TOPS_   3072,1960,2372,1476,2097,1572,1920,1570,1777,1524,1646,1454,1286,1369,1234,1115,1011,918,837,894,823,759,702,650,603,560,522,487,455,425,398,374,351,330,310,292,275,259,280,265,251,266,253,240,252,240,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
#define PWM1_LEVELS   _PWM1_LEVELS_,_PWM2_TOPS_
#define PWM2_LEVELS   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,_PWM2_LEVELS_
#define PWM_TOPS      _PWM1_TOPS_,_PWM2_TOPS_

#define MAX_1x7135         15
#define HALFSPEED_LEVEL    15
#define QUARTERSPEED_LEVEL 15
#define DEFAULT_LEVEL      50

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     150
// 1 25 50 [75] 100 125 150
#define RAMP_DISCRETE_FLOOR  1
#define RAMP_DISCRETE_CEIL   RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS  7

// at Sofirn's request, use max (150) for the Simple UI ceiling
// 15 48 [82] 116 150
#define SIMPLE_UI_FLOOR  MAX_1x7135
#define SIMPLE_UI_CEIL   150
#define SIMPLE_UI_STEPS   5

// turn on at ~6 lm by default (level 50/150, or ramp step 2/5 or 3/7)
// (also sets lockout mode 2H to a useful level)
#define DEFAULT_MANUAL_MEMORY 50
// reset to default after being off for 10 minutes
#define DEFAULT_MANUAL_MEMORY_TIMER 10

// enable SOS in the blinkies group
#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE

// and finally, set the default ramp style to Stepped
#undef RAMP_STYLE
#define RAMP_STYLE 1  // 0 = smooth, 1 = stepped

// stop panicking at ~30% power
#define THERM_FASTER_LEVEL 105
#define MIN_THERM_STEPDOWN 65  // must be > end of dynamic PWM range

// slow down party strobe; this driver can't pulse for too short a time
//#define PARTY_STROBE_ONTIME 8
#define STROBE_OFF_LEVEL 1  // keep the regulator chip on between pulses

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 50

// enable 2 click turbo (replaces USE_2C_MAX_TURBO)
#define DEFAULT_2C_STYLE 1

// don't blink during the ramp or at the ceiling
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif
#ifdef BLINK_AT_RAMP_CEIL
#undef BLINK_AT_RAMP_CEIL
#endif


// enable factory reset on 13H without loosening tailcap
#define USE_SOFT_FACTORY_RESET

