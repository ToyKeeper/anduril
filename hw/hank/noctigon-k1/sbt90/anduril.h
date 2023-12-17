// Noctigon K1-SBT90.2 config options for Anduril
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// (is a K1 host with a KR4-like driver and a really high-powered LED)
#include "hank/noctigon-k1/sbt90/hwdef.h"
#include "hank/anduril.h"

// this light can safely run a bit hotter than most
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 55

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
#define USE_AUX_RGB_LEDS_WHILE_ON  10
#define USE_CONFIGURABLE_RGB_VOLTAGE_LEVELS
#define USE_INDICATOR_LED_WHILE_RAMPING


// brightness:
// 0/1023: 0.35 lm
// 1/1023: 2.56 lm
// max regulated: 1740 lm
// FET: ~3700 lm
#define RAMP_SIZE 150
// FIXME: it should probably have max_regulated at 120, not 130
// ramp copied from noctigon-kr4
// nice low lows, but might have visible ripple on some lights:
// maxreg at 130, dynamic PWM: level_calc.py 5.01 2 149 7135 1 0.3 1740 FET 1 10 3190 --pwm dyn:64:16384:255
// (plus one extra level at the beginning for moon)
#define PWM1_LEVELS  0,1,1,2,2,3,4,5,6,7,8,9,11,12,14,16,17,19,22,24,26,29,31,34,37,40,43,46,49,53,56,60,63,67,71,74,78,82,86,89,93,96,99,103,105,108,110,112,114,115,116,116,115,114,112,109,106,101,95,89,81,71,60,48,34,19,20,21,22,23,24,26,27,28,30,31,32,34,36,37,39,41,43,45,47,49,51,53,56,58,61,63,66,69,72,75,78,81,84,88,91,95,99,103,107,111,115,119,124,129,133,138,143,149,154,159,165,171,177,183,189,196,203,210,217,224,231,239,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,20,30,41,52,63,75,87,99,112,125,138,151,165,179,194,208,224,239,255
#define PWM_TOPS     16383,16383,11750,14690,9183,12439,13615,13955,13877,13560,13093,12529,13291,12513,12756,12769,11893,11747,12085,11725,11329,11316,10851,10713,10518,10282,10016,9729,9428,9298,8971,8794,8459,8257,8043,7715,7497,7275,7052,6753,6538,6260,5994,5798,5501,5271,5006,4758,4525,4268,4030,3775,3508,3263,3010,2752,2517,2256,1998,1763,1512,1249,994,749,497,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
#define MIN_THERM_STEPDOWN 66  // should be above highest dyn_pwm level

#define MAX_1x7135 130
#define DEFAULT_LEVEL 50
#define HALFSPEED_LEVEL 12
#define QUARTERSPEED_LEVEL 4

#if 0  // original 10-bit ramp
// maxreg at 130: level_calc.py cube 2 150 7135 0 2.5 1740 FET 1 10 2565
// maxreg at 120: level_calc.py cube 2 150 7135 0 2.5 1740 FET 1 10 3190
#define PWM1_LEVELS 0,0,1,1,2,2,3,3,4,4,5,6,7,8,9,10,11,13,14,15,17,19,20,22,24,26,28,30,33,35,38,40,43,46,49,52,55,59,62,66,70,74,78,82,86,91,96,100,105,111,116,121,127,133,139,145,151,158,165,172,179,186,193,201,209,217,225,234,243,251,261,270,280,289,299,310,320,331,342,353,364,376,388,400,412,425,438,451,464,478,492,506,521,536,551,566,582,597,614,630,647,664,681,699,717,735,754,772,792,811,831,851,871,892,913,935,956,978,1001,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,51,79,109,138,168,198,229,260,292,324,357,390,423,457,492,527,562,598,634,671,708,746,784,822,861,901,941,982,1023
#define DEFAULT_LEVEL 46
#define MAX_1x7135 120
#define HALFSPEED_LEVEL 10
#define QUARTERSPEED_LEVEL 2
#endif

#define RAMP_SMOOTH_FLOOR 11  // low levels may be unreliable
#define RAMP_SMOOTH_CEIL  130
// 11 30 [50] 70 90 110 [130]
#define RAMP_DISCRETE_FLOOR 11
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit ~33% power
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL 120
#define SIMPLE_UI_STEPS 5

// stop panicking at ~40% power or ~1700 lm
#define THERM_FASTER_LEVEL 130
//#define THERM_NEXT_WARNING_THRESHOLD 16  // accumulate less error before adjusting
//#define THERM_RESPONSE_MAGNITUDE 128  // bigger adjustments

#define THERM_CAL_OFFSET 5

// the power regulator is a bit slow, so push it harder for a quick response from off
// (unsure if necessary, copied from noctigon-kr4)
//#define DEFAULT_JUMP_START_LEVEL 21
//#define BLINK_BRIGHTNESS DEFAULT_LEVEL
//#define BLINK_ONCE_TIME 12

// there is usually no lighted button, so
// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN

// normal party strobe speed; this driver can pulse very quickly due to its DD FET
//#define PARTY_STROBE_ONTIME 2

// the default of 26 looks a bit rough, so increase it to make it smoother
#define CANDLE_AMPLITUDE 33

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// for compatibility with other models
#define USE_SOFT_FACTORY_RESET

