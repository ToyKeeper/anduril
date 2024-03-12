// Anduril config for Emisar 2ch+FET-joined-into-1-channel
// Copyright (C) 2024 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// This is mostly just used on the Emisar D4S w/ lighted switch,
// which uses a 2-channel driver but only actually uses 1 set of LEDs
// (so instead of the usual 2ch or 2ch-fet firmware, it needs something else)

#define HWDEF_H  hank/emisar-2ch/fet/joined/hwdef.h
#include "hank/anduril.h"

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// the aux LEDs are front-facing, so turn them off while main LEDs are on
// it also has an independent LED in the button
#define USE_BUTTON_LED
// enabling this option breaks the button LED
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

// channel modes...
// CM_CH1      -- linear ch1 only
// CM_CH1_FET  -- linear ch1 + DD FET
#define DEFAULT_CHANNEL_MODE           CM_CH1_FET

#define USE_CONFIG_COLORS

// strobes on this light should use the same channel as the ramp
#undef USE_CHANNEL_PER_STROBE

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL          CM_CH1_FET

#define POLICE_COLOR_STROBE_CH1        CM_AUXRED
#define POLICE_COLOR_STROBE_CH2        CM_AUXBLU

#define RAMP_SIZE 150

// Output estimates:
// - linear ch1 only: 2500 lm
// - ch1 + DD FET: 3500 lm
//   (tested on a D4S with Osram W2 LEDs, with a mediocre battery at 3.7V)
//   (LuxWad estimated 6000 lm (uncalibrated) on a full high-amp cell)
//   (I picked 4500 lm as a rough realistic estimate)
// - linear ch2 only: 0 (not connected)
// - moon: 0.7 lm at PWM 1/4096, 0.15 lm at DSM 0/32640

// delta-sigma modulated PWM (0b0HHHHHHHHLLLLLLL = 0, 8xHigh, 7xLow bits)
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
//
// linear ch1 only (copied from noctigon-m44 ramp)
// level_calc.py 5.01 1 150 7135 0 0.2 2000 --pwm 32640
#define PWM1_LEVELS 0,1,2,3,4,5,6,7,9,10,12,14,17,19,22,25,28,32,36,41,45,50,56,62,69,76,84,92,101,110,121,132,143,156,169,184,199,215,232,251,270,291,313,336,360,386,414,442,473,505,539,574,612,651,693,736,782,829,880,932,987,1045,1105,1168,1233,1302,1374,1449,1527,1608,1693,1781,1873,1969,2068,2172,2279,2391,2507,2628,2753,2883,3018,3158,3303,3454,3609,3771,3938,4111,4289,4475,4666,4864,5068,5280,5498,5724,5957,6197,6445,6701,6965,7237,7518,7808,8106,8413,8730,9056,9392,9737,10093,10459,10835,11223,11621,12031,12452,12884,13329,13786,14255,14737,15232,15741,16262,16798,17347,17911,18489,19082,19691,20314,20954,21609,22281,22969,23674,24397,25137,25895,26671,27465,28279,29111,29963,30835,31727,32640
//
// linear+FET linear ch1 segment
// level_calc.py 4.85 2 150 7135 0 0.5 2500 FET 1 10 4500 --pwm 32640
#define PWM2_LEVELS 0,2,3,5,7,9,11,14,17,20,24,28,33,38,44,50,57,64,72,81,91,101,112,125,138,152,167,184,201,220,241,263,286,311,337,365,395,427,461,497,535,575,618,663,711,761,814,870,929,992,1057,1126,1198,1274,1353,1437,1524,1616,1712,1812,1917,2027,2141,2261,2386,2516,2652,2794,2941,3095,3255,3422,3595,3775,3962,4156,4358,4568,4785,5011,5245,5487,5738,5999,6268,6548,6837,7135,7445,7764,8095,8437,8790,9154,9531,9919,10321,10735,11162,11602,12056,12524,13006,13503,14015,14542,15085,15643,16218,16810,17419,18045,18688,19350,20031,20730,21448,22186,22944,23722,24521,25342,26184,27048,27935,28844,29777,30733,31714,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,32640,0
//
// linear+FET DD FET segment
// level_calc.py 4.85 2 150 7135 0 0.5 2500 FET 1 10 4500 --pwm 255
#define PWM3_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,20,31,42,53,65,76,88,101,113,126,139,152,166,180,194,209,224,239,255

#define DEFAULT_LEVEL      70
#define MIN_THERM_STEPDOWN 50
#define MAX_REGULATED      130
#define MAX_1x7135         MAX_REGULATED
#define MAX_Nx7135         MAX_REGULATED
#define HALFSPEED_LEVEL    20
#define QUARTERSPEED_LEVEL 10

#define RAMP_SMOOTH_FLOOR  1
#define RAMP_SMOOTH_CEIL   150
// 10, 30, 50, [70], 90, 110, [130]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  MAX_REGULATED
#define RAMP_DISCRETE_STEPS 7

// safe limit highest regulated power (no FET or turbo)
// 10, 40, [70], 100, [130]
#define SIMPLE_UI_FLOOR  RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL   RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS  5

// stop panicking at ~2500 lm
#define THERM_FASTER_LEVEL MAX_REGULATED

#define USE_POLICE_COLOR_STROBE_MODE
#undef  TACTICAL_LEVELS
#define TACTICAL_LEVELS MAX_REGULATED,30,(RAMP_SIZE+3)  // high, low, police strobe

// use the brightest setting for strobe
#define STROBE_BRIGHTNESS MAX_LEVEL
// slow down party strobe; this driver can't pulse for less than 1ms
// (in linear-only mode ... but FET mode can pulse a much shorter time)
#define PARTY_STROBE_ONTIME 1

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 33

// the power regulator is a bit slow, so push it harder for a quick response from off
#define DEFAULT_JUMP_START_LEVEL 22
#define BLINK_BRIGHTNESS 45
#define BLINK_ONCE_TIME 12  // longer blink, since main LEDs are slow

#define THERM_CAL_OFFSET 5

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

