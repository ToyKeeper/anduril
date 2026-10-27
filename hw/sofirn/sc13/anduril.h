// Sofirn SC13, modelled after the TS25 and TS10 with RGB Aux, but with a buck regulator
// Copyright (C) 2024
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define HWDEF_H  sofirn/sc13/hwdef.h
#include "wurkkos/anduril.h"

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// don't turn on the aux LEDs while main LEDs are on
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

#define RAMP_SIZE 150

// Use 4/4096 as the lowest level (~3 lm) ... anything lower than that seems to flicker
// level_calc.py 5.8 1 150 7135 4 3 1800 --pwm dyn:74:4096:255
#define PWM1_LEVELS  4,5,5,5,6,6,7,9,9,10,11,12,13,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,30,31,32,33,34,35,37,38,38,39,40,41,42,44,45,46,47,48,48,49,50,51,51,52,53,53,53,53,53,53,53,53,52,52,51,50,49,47,46,44,41,39,36,33,30,27,28,29,30,31,32,33,34,35,37,38,39,40,42,43,45,46,48,49,51,52,54,56,58,59,61,63,65,67,70,72,74,76,79,81,84,86,89,91,94,97,100,103,106,109,112,116,119,123,126,130,134,137,141,145,149,154,158,162,167,171,176,181,186,191,196,202,207,213,218,224,230,236,242,249,255
#define PWM_TOPS     4095,2941,3765,2556,3480,2623,3210,3517,2950,3192,3334,3414,3453,3088,3125,3137,3130,3110,3080,3042,2998,2948,2896,2840,2783,2723,2663,2603,2542,2584,2518,2453,2388,2325,2263,2273,2210,2148,2088,2030,1973,1918,1912,1858,1805,1754,1704,1619,1575,1531,1489,1418,1380,1343,1280,1222,1166,1114,1064,1017,973,912,872,818,767,719,658,617,563,500,455,399,348,300,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255


#define MAX_1x7135 75
#define DEFAULT_LEVEL 50
#define MIN_THERM_STEPDOWN 60
#define HALFSPEED_LEVEL 20
#define QUARTERSPEED_LEVEL 5

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     150
// 20 38 56 [75] 93 111 130
#define RAMP_DISCRETE_FLOOR  5
#define RAMP_DISCRETE_CEIL   130
#define RAMP_DISCRETE_STEPS  7

// 25 50 [75] 100 125
#define SIMPLE_UI_FLOOR  5
#define SIMPLE_UI_CEIL   150
#define SIMPLE_UI_STEPS  5

// and finally, set the default ramp style to Stepped
#undef RAMP_STYLE
#define RAMP_STYLE 1  // 0 = smooth, 1 = stepped

#define STROBE_OFF_LEVEL 1  // keep the regulator chip on between pulses

// use the brightest setting for strobe
#define STROBE_BRIGHTNESS MAX_LEVEL
// slow down party strobe; this driver can't pulse very fast
#define PARTY_STROBE_ONTIME 12
#define BIKE_STROBE_ONTIME 8

#define BLINK_BRIGHTNESS 30
#define BLINK_ONCE_TIME 48  // longer blink, since LEDs are slow
#define BLIP_LEVEL 10
#define BUZZ_SPEED 36 // default buzz of 16ms is too fast to turn LEDs on, slow it down to 36ms

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 42

// stop panicking at ~50% power
#define THERM_FASTER_LEVEL 120  // throttle back faster when high
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 50

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default
// (so battcheck will be visible while charging)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN
// blink numbers on the aux LEDs by default
//#define DEFAULT_BLINK_CHANNEL  CM_AUXWHT

// Just blink volts plus tenths, not the extra digit
#ifdef USE_EXTRA_BATTCHECK_DIGIT
#undef USE_EXTRA_BATTCHECK_DIGIT
#endif

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// Barry isn't a fan of the voltage-based aux for fear of red LEDs drawing customer concerns, so override the default
#ifdef RGB_LED_OFF_DEFAULT
#undef RGB_LED_OFF_DEFAULT
#endif
#define RGB_LED_OFF_DEFAULT 0x13  // 0x19 = low, voltage;  0x13 = low, cyan; 0x12 = low, green; 0x14 = low, blue
//#define RGB_LED_LOCKOUT_DEFAULT 0x39  // blinking, voltage