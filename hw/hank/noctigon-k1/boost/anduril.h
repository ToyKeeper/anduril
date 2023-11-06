// Noctigon K1 boost config options for Anduril
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "hank/noctigon-k1/boost/hwdef.h"
#include "hank/anduril.h"

// Most K1 lights can run hotter than default, but the 12V model
// is known to have issues with the driver itself getting too hot.
// This then causes the main LEDs to turn off during use, because the
// boost chip's built-in protection activates.  So try to keep the
// temperature relatively low.
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 45

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
#define USE_AUX_RGB_LEDS_WHILE_ON  25
#define USE_CONFIGURABLE_RGB_VOLTAGE_LEVELS
#define USE_INDICATOR_LED_WHILE_RAMPING

#if 0  // old, 10-bit PWM method
// level_calc.py cube 1 150 7135 0 4 1300
// (with max_pwm set to 1023)
// (level 0 is usable on this light)
#define RAMP_LENGTH 150
#define PWM1_LEVELS 0,0,1,1,2,2,3,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,17,19,20,22,23,25,26,28,30,31,33,35,37,39,42,44,46,48,51,53,56,59,61,64,67,70,73,76,80,83,86,90,94,97,101,105,109,113,117,122,126,130,135,140,144,149,154,159,165,170,175,181,187,193,198,204,211,217,223,230,236,243,250,257,264,271,279,286,294,302,310,318,326,334,343,351,360,369,378,387,397,406,416,426,436,446,456,466,477,488,499,510,521,532,544,555,567,579,591,604,616,629,642,655,668,682,695,709,723,737,751,766,780,795,810,825,841,856,872,888,904,921,937,954,971,988,1005,1023
#define MAX_1x7135 50
// don't slow down at low levels; this isn't that sort of light
// (it needs to stay at full speed for the 10-bit PWM to work)
#ifdef USE_DYNAMIC_UNDERCLOCKING
#undef USE_DYNAMIC_UNDERCLOCKING
#endif
#endif

#define RAMP_SIZE 150
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

#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  130
// 10, 30, 50, [70], 90, 110, 130
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit ~50% power
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL 120
#define SIMPLE_UI_STEPS 5

// stop panicking at ~70% power or ~600 lm
#define THERM_FASTER_LEVEL 130

#define THERM_CAL_OFFSET 5

#define THERM_RESPONSE_MAGNITUDE 32  // smaller adjustments, this host changes temperature slowly
#define THERM_NEXT_WARNING_THRESHOLD 32  // more error tolerance before adjusting

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN

// slow down party strobe; this driver can't pulse for 1ms or less
#define PARTY_STROBE_ONTIME 4

// use aux red + aux blue for police strobe
#define USE_POLICE_COLOR_STROBE_MODE
#define POLICE_STROBE_USES_AUX
#define POLICE_COLOR_STROBE_CH1        CM_AUXRED
#define POLICE_COLOR_STROBE_CH2        CM_AUXBLU

// make candle mode wobble more
#define CANDLE_AMPLITUDE 33

// the entire ramp is regulated; don't blink halfway up
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// added for convenience
#define USE_SOFT_FACTORY_RESET

