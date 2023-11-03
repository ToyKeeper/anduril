// Emisar D4K 3-channel config options for Anduril
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "hank/emisar-d4k-3ch/hwdef.h"
#include "hank/anduril.h"

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// turn on the aux LEDs while main LEDs are on
// (in case there's a RGB button)
#define USE_AUX_RGB_LEDS_WHILE_ON  40
#define USE_INDICATOR_LED_WHILE_RAMPING

// channel modes...
// CM_MAIN2, CM_LED3, CM_LED4, CM_ALL,
// CM_BLEND34A, CM_BLEND34B, CM_HSV, CM_AUTO3
#define DEFAULT_CHANNEL_MODE           CM_ALL

#define FACTORY_RESET_WARN_CHANNEL     CM_LED4
#define FACTORY_RESET_SUCCESS_CHANNEL  CM_MAIN2

#define CONFIG_WAITING_CHANNEL         CM_LED3
#define CONFIG_BLINK_CHANNEL           CM_ALL

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL          CM_MAIN2

// LEDs 3 and 4 make a nice police strobe
#define POLICE_COLOR_STROBE_CH1        CM_LED3
#define POLICE_COLOR_STROBE_CH2        CM_LED4
// aux red + aux blue are the correct colors, but are dim
//#define POLICE_COLOR_STROBE_CH1        CM_AUXRED
//#define POLICE_COLOR_STROBE_CH2        CM_AUXBLU

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
#define TINT_RAMPING_CORRECTION 0  // none, linear regulator doesn't need it

// main 2 LEDs
//   output: unknown, 2000 lm?
// LED 3 / 4
//   output: unknown, 1000 lm each?
#define RAMP_SIZE 150
// delta-sigma modulated PWM (0b0HHHHHHHHLLLLLLL = 0, 8xHigh, 7xLow bits)
// level_calc.py 5.01 1 150 7135 0 0.2 2000 --pwm 32640
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
#define PWM1_LEVELS 0,1,2,3,4,5,6,7,9,10,12,14,17,19,22,25,28,32,36,41,45,50,56,62,69,76,84,92,101,110,121,132,143,156,169,184,199,215,232,251,270,291,313,336,360,386,414,442,473,505,539,574,612,651,693,736,782,829,880,932,987,1045,1105,1168,1233,1302,1374,1449,1527,1608,1693,1781,1873,1969,2068,2172,2279,2391,2507,2628,2753,2883,3018,3158,3303,3454,3609,3771,3938,4111,4289,4475,4666,4864,5068,5280,5498,5724,5957,6197,6445,6701,6965,7237,7518,7808,8106,8413,8730,9056,9392,9737,10093,10459,10835,11223,11621,12031,12452,12884,13329,13786,14255,14737,15232,15741,16262,16798,17347,17911,18489,19082,19691,20314,20954,21609,22281,22969,23674,24397,25137,25895,26671,27465,28279,29111,29963,30835,31727,32640

#define MIN_THERM_STEPDOWN 50
#define DEFAULT_LEVEL 70
#define MAX_1x7135 70
// always run at 1/4th speed, because 4 kHz PWM is enough for this circuit
// and speed changes make a big visible bump
#define HALFSPEED_LEVEL     255
#define QUARTERSPEED_LEVEL  255

#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  130
// 10, 30, 50, [70], 90, 110, 130
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// 10 40 [70] 100 130
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS 5

// stop panicking at ~1500 lm
#define THERM_FASTER_LEVEL 130

#define USE_POLICE_COLOR_STROBE_MODE
#undef  TACTICAL_LEVELS
#define TACTICAL_LEVELS 120,30,(RAMP_SIZE+3)  // high, low, police strobe

// use the brightest setting for strobe
#define STROBE_BRIGHTNESS MAX_LEVEL
// slow down party strobe; this driver can't pulse for 1ms or less
#define PARTY_STROBE_ONTIME 2
// #define STROBE_OFF_LEVEL 1  // nope, this makes strobe blurry
// bike strobe needs a longer pulse too?
//#define BIKE_STROBE_ONTIME 8

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 33

// the power regulator is a bit slow, so push it harder for a quick response from off
#define DEFAULT_JUMP_START_LEVEL 21
#define BLINK_BRIGHTNESS 50
#define BLINK_ONCE_TIME 12  // longer blink, since main LEDs are slow

#define THERM_CAL_OFFSET 5

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// for consistency with KR4 (not otherwise necessary though)
#define USE_SOFT_FACTORY_RESET

