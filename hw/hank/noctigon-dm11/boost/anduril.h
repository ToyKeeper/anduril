// Noctigon DM11 (boost driver) config options for Anduril
// Copyright (C) 2021-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "hank/noctigon-dm11/boost/hwdef.h"
#include "hank/anduril.h"

#define RAMP_SIZE 150

// power channels:
// - boost: 8A?
// - DD FET: none (can't do DD on a boost driver)

#if 0  // old, dynamic PWM method
// level_calc.py 5.01 1 149 7135 1 0.3 1740 --pwm dyn:78:16384:255
// (plus a 0 at the beginning for moon)
#define PWM1_LEVELS 0,1,1,1,2,3,3,4,5,6,7,8,9,10,11,13,14,16,17,19,21,23,25,27,29,31,34,36,39,42,44,47,50,53,57,60,63,67,70,74,77,81,85,88,92,96,99,103,107,110,113,117,120,123,126,128,130,133,134,136,137,137,137,137,136,135,133,130,126,122,117,111,104,96,87,76,65,52,38,22,23,25,26,27,28,29,30,32,33,34,36,37,39,40,42,43,45,47,49,51,53,55,57,59,61,63,66,68,70,73,76,78,81,84,87,90,93,96,99,103,106,110,113,117,121,125,129,133,137,142,146,151,155,160,165,170,175,181,186,192,197,203,209,215,222,228,234,241,248,255
#define PWM_TOPS 16383,16383,12404,8140,11462,14700,11041,12947,13795,14111,14124,13946,13641,13248,12791,13418,12808,13057,12385,12428,12358,12209,12000,11746,11459,11147,11158,10793,10708,10576,10173,9998,9800,9585,9527,9278,9023,8901,8634,8486,8216,8053,7881,7615,7440,7261,7009,6832,6656,6422,6196,6031,5819,5615,5419,5190,4973,4803,4571,4386,4179,3955,3745,3549,3340,3145,2940,2729,2513,2312,2109,1903,1697,1491,1286,1070,871,662,459,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
#define MAX_1x7135 150
#define DEFAULT_LEVEL 70
#define MIN_THERM_STEPDOWN 80  // must be > end of dynamic PWM range
#define HALFSPEED_LEVEL 12
#define QUARTERSPEED_LEVEL 4
#endif

// delta-sigma modulated PWM (0b0HHHHHHHHLLLLLLL = 0, 8xHigh, 7xLow bits)
// level_calc.py 5.01 1 150 7135 0 0.2 2000 --pwm 32640
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
#define PWM1_LEVELS 0,1,2,3,4,5,6,7,9,10,12,14,17,19,22,25,28,32,36,41,45,50,56,62,69,76,84,92,101,110,121,132,143,156,169,184,199,215,232,251,270,291,313,336,360,386,414,442,473,505,539,574,612,651,693,736,782,829,880,932,987,1045,1105,1168,1233,1302,1374,1449,1527,1608,1693,1781,1873,1969,2068,2172,2279,2391,2507,2628,2753,2883,3018,3158,3303,3454,3609,3771,3938,4111,4289,4475,4666,4864,5068,5280,5498,5724,5957,6197,6445,6701,6965,7237,7518,7808,8106,8413,8730,9056,9392,9737,10093,10459,10835,11223,11621,12031,12452,12884,13329,13786,14255,14737,15232,15741,16262,16798,17347,17911,18489,19082,19691,20314,20954,21609,22281,22969,23674,24397,25137,25895,26671,27465,28279,29111,29963,30835,31727,32640
#define MIN_THERM_STEPDOWN 50
#define DEFAULT_LEVEL 70
#define MAX_1x7135 150
// moon at 1/4th speed for max runtime
// the rest at half speed, to reduce ripple
// but not full speed because 8 kHz PWM is enough for this circuit
// and speed changes make a big visible bump
#define HALFSPEED_LEVEL     255
#define QUARTERSPEED_LEVEL  2

#define RAMP_SMOOTH_FLOOR 1  // low levels may be unreliable
#define RAMP_SMOOTH_CEIL  130
// 10, 30, 50, [70], 90, 110, 130
// Nichia B35 model: (0.56), 1.4, 8.4, 34.5, [102], 250, 500, 860, (1300) lm
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit ~75% power
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS 5

// stop panicking at ~70% power or ~600 lm
#define THERM_FASTER_LEVEL 130

#define THERM_CAL_OFFSET 5

// the power regulator seems to "jump start" the LEDs all on its own,
// so the firmware doesn't have to
// (and unfortunately the power regulator jumps it a bit too hard)
#define DEFAULT_JUMP_START_LEVEL 1
#define BLINK_BRIGHTNESS 50
#define BLINK_ONCE_TIME 12

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN

// slow down party strobe; this driver can't pulse for 2ms or less
#define PARTY_STROBE_ONTIME 3

// use aux red + aux blue for police strobe
#define USE_POLICE_COLOR_STROBE_MODE
#define POLICE_STROBE_USES_AUX
#define POLICE_COLOR_STROBE_CH1        CM_AUXRED
#define POLICE_COLOR_STROBE_CH2        CM_AUXBLU

// the default of 26 looks a bit rough, so increase it to make it smoother
#define CANDLE_AMPLITUDE 33

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// added for convenience
#define USE_SOFT_FACTORY_RESET

