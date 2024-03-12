// Wurkkos TS25 Boost
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define HWDEF_H  wurkkos/ts25/boost/hwdef.h
#include "wurkkos/anduril.h"

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// don't turn on the aux LEDs while main LEDs are on
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

#define VOLTAGE_FUDGE_FACTOR 5  // add 0.25V, not 0.35V

#define RAMP_SIZE 150

// delta-sigma modulated PWM (0b0HHHHHHHHLLLLLLL = 0, 8xHigh, 7xLow bits)
// level_calc.py 5.01 1 150 7135 0 0.2 2000 --pwm 32640
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
#define PWM1_LEVELS 0,1,2,3,4,5,6,7,9,10,12,14,17,19,22,25,28,32,36,41,45,50,56,62,69,76,84,92,101,110,121,132,143,156,169,184,199,215,232,251,270,291,313,336,360,386,414,442,473,505,539,574,612,651,693,736,782,829,880,932,987,1045,1105,1168,1233,1302,1374,1449,1527,1608,1693,1781,1873,1969,2068,2172,2279,2391,2507,2628,2753,2883,3018,3158,3303,3454,3609,3771,3938,4111,4289,4475,4666,4864,5068,5280,5498,5724,5957,6197,6445,6701,6965,7237,7518,7808,8106,8413,8730,9056,9392,9737,10093,10459,10835,11223,11621,12031,12452,12884,13329,13786,14255,14737,15232,15741,16262,16798,17347,17911,18489,19082,19691,20314,20954,21609,22281,22969,23674,24397,25137,25895,26671,27465,28279,29111,29963,30835,31727,32640
#define MIN_THERM_STEPDOWN 50
#define DEFAULT_LEVEL 70
#define MAX_1x7135 150
// moon at 1/4th speed for max runtime
// the rest at full speed, to reduce PWM
// (speed changes make a big visible bump because PWM)
#define HALFSPEED_LEVEL      2
#define QUARTERSPEED_LEVEL   2

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     150
// 10, 30, 50, [70], 90, 110, 130
#define RAMP_DISCRETE_FLOOR  10
#define RAMP_DISCRETE_CEIL   130
#define RAMP_DISCRETE_STEPS  7

// at Wurkkos's request, reduce the Simple UI ceiling a little bit
// 25 50 [75] 100 125
#define SIMPLE_UI_FLOOR  25
#define SIMPLE_UI_CEIL   125
#define SIMPLE_UI_STEPS  5

// stop panicking at ~50% power
#define THERM_FASTER_LEVEL 120  // throttle back faster when high

#define BLINK_BRIGHTNESS 50
#define BLINK_ONCE_TIME 12

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default
// (so battcheck will be visible while charging)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN
// blink numbers on the aux LEDs by default
//#define DEFAULT_BLINK_CHANNEL  CM_AUXWHT

// slow down party strobe; this driver can't pulse for 2ms or less
#define PARTY_STROBE_ONTIME 1
//#define STROBE_OFF_LEVEL 1  // keep the regulator chip on between pulses

// use aux red + aux blue for police strobe
#define USE_POLICE_COLOR_STROBE_MODE
#define POLICE_STROBE_USES_AUX
#define POLICE_COLOR_STROBE_CH1        CM_AUXRED
#define POLICE_COLOR_STROBE_CH2        CM_AUXBLU

// the default of 26 looks a bit rough, so increase it to make it smoother
#define CANDLE_AMPLITUDE 33

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

