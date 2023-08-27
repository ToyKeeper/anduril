// Emisar D4K 3-channel config options for Anduril
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define MODEL_NUMBER "0151"
#include "hwdef-emisar-d4k-3ch.h"
#include "hank-cfg.h"
// ATTINY: 1634

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// turn on the aux LEDs while main LEDs are on
// (in case there's a RGB button)
#define USE_AUX_RGB_LEDS_WHILE_ON  25
#define USE_INDICATOR_LED_WHILE_RAMPING

// channel modes...
// CM_MAIN2, CM_LED3, CM_LED4, CM_ALL,
// CM_BLEND34A, CM_BLEND34B, CM_HSV, CM_AUTO3
#define DEFAULT_CHANNEL_MODE           CM_MAIN2

#define FACTORY_RESET_WARN_CHANNEL     CM_LED4
#define FACTORY_RESET_SUCCESS_CHANNEL  CM_MAIN2

#define USE_CONFIG_COLORS
#define CONFIG_WAITING_CHANNEL         CM_LED3
#define CONFIG_BLINK_CHANNEL           CM_ALL

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN2

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
// level_calc.py 5.01 1 150 7135 1 0.2 2000 --pwm dyn:69:16383:511
// 8-bit PWM
#define PWM1_LEVELS 0,1,1,2,2,2,2,3,3,3,3,4,4,5,5,6,6,6,7,8,8,9,9,10,10,11,12,13,13,14,15,16,16,17,18,19,20,21,22,23,23,24,26,27,28,29,30,31,32,33,34,36,37,38,39,41,42,43,45,46,47,49,50,52,53,55,56,58,59,61,62,64,66,67,69,71,72,74,76,78,80,81,83,85,87,89,91,93,95,97,99,101,103,105,107,109,111,113,116,118,120,122,125,127,129,132,134,136,139,141,144,146,148,151,154,156,159,161,164,166,169,172,174,177,180,183,185,188,191,194,197,200,203,205,208,211,214,217,220,223,226,230,233,236,239,242,245,249,252,255
// dynamic PWM
#define PWM2_LEVELS 0,1,1,2,2,2,3,4,4,5,6,6,7,8,9,10,11,13,14,15,17,18,20,21,23,25,27,29,31,33,35,38,40,42,45,47,50,52,55,57,60,62,65,67,70,72,74,76,78,79,81,82,83,84,84,85,84,84,82,81,78,76,72,68,63,58,51,44,35,26,27,29,30,32,33,35,37,38,40,42,44,46,48,50,53,55,57,60,63,65,68,71,74,77,80,83,87,90,94,98,102,106,110,114,118,123,128,132,137,142,148,153,159,164,170,176,183,189,196,202,209,216,224,231,239,247,255,263,272,281,290,299,309,318,328,339,349,360,371,382,394,406,418,430,443,456,469,483,497,511
#define PWM_TOPS 16383,13662,10727,15413,11905,8157,12776,14730,12250,13437,13990,11928,12275,12360,12270,12060,11765,12450,11970,11470,11698,11138,11181,10600,10534,10404,10226,10014,9776,9519,9248,9220,8919,8617,8512,8203,8066,7760,7605,7308,7144,6860,6693,6426,6260,6009,5769,5539,5320,5046,4851,4607,4378,4163,3914,3727,3468,3268,3008,2803,2548,2345,2099,1874,1642,1430,1189,970,728,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511
#define MIN_THERM_STEPDOWN 70  // should be above highest dyn_pwm level
#define DEFAULT_LEVEL 70
#define MAX_1x7135 70
#define HALFSPEED_LEVEL 10
#define QUARTERSPEED_LEVEL 4

//#define DEFAULT_MANUAL_MEMORY        DEFAULT_LEVEL
//#define DEFAULT_MANUAL_MEMORY_TIMER  10

#define RAMP_SMOOTH_FLOOR 10
#define RAMP_SMOOTH_CEIL  130
// 10, 30, 50, [70], 90, 110, [130]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit highest regulated power (no FET or turbo)
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
// bike strobe needs a longer pulse too
#define BIKE_STROBE_ONTIME 8

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

