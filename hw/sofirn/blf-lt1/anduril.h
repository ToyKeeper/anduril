// BLF Lantern config options for Anduril
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "sofirn/blf-lt1/hwdef.h"

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
//#define DEFAULT_BLINK_CHANNEL  CM_BOTH  // takes too much space

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
//#define TINT_RAMPING_CORRECTION 26  // prototype, 140%
//#define TINT_RAMPING_CORRECTION 10  // production model, 115%
#define TINT_RAMPING_CORRECTION 0  // none

#define RAMP_SIZE 150
// delta-sigma modulated PWM (0b0HHHHHHHHLLLLLLL = 0, 8xHigh, 7xLow bits)
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
// level_calc.py 3.333 1 150 7135 32 0.2 600 --pwm 32640
#define PWM1_LEVELS  32,35,38,41,45,50,55,61,67,74,82,91,100,110,121,133,146,160,175,192,209,227,247,268,291,314,340,366,395,424,456,489,524,560,599,639,681,726,772,820,871,924,979,1036,1096,1158,1222,1289,1359,1431,1506,1584,1664,1747,1834,1923,2015,2111,2209,2311,2416,2524,2636,2751,2870,2992,3118,3247,3380,3518,3659,3803,3952,4105,4262,4423,4589,4759,4933,5111,5294,5482,5674,5871,6073,6279,6491,6707,6928,7155,7386,7623,7865,8113,8365,8624,8888,9157,9432,9713,10000,10292,10591,10895,11206,11523,11846,12175,12511,12853,13202,13557,13919,14287,14663,15045,15434,15830,16233,16644,17061,17486,17919,18358,18805,19260,19723,20193,20671,21156,21650,22152,22662,23180,23706,24241,24784,25335,25895,26464,27041,27627,28222,28826,29439,30060,30691,31332,31981,32640

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

// LT1 can handle heat well, so don't limit simple mode
#define SIMPLE_UI_FLOOR  10
#define SIMPLE_UI_CEIL   150
#define SIMPLE_UI_STEPS  5

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE
#define USE_EXTENDED_SIMPLE_UI

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

