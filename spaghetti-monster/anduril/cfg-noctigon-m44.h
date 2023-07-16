// Noctigon M44 config options for Anduril
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define MODEL_NUMBER "0143"
#include "hwdef-noctigon-m44.h"
#include "hank-cfg.h"
// ATTINY: 1634

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// the aux LEDs are front-facing, so turn them off while main LEDs are on
// it also has an independent LED in the button
#define USE_BUTTON_LED
// TODO: the whole "indicator LED" thing needs to be refactored into
//       "aux LED(s)" and "button LED(s)" since they work a bit differently
// enabling this option breaks the button LED on D4v2.5
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

// channel modes...
// CM_CH1, CM_CH2, CM_BOTH, CM_BLEND, CM_AUTO
// enable max brightness out of the box
#define DEFAULT_CHANNEL_MODE           CM_BOTH

//#define FACTORY_RESET_WARN_CHANNEL     CM_CH2
//#define FACTORY_RESET_SUCCESS_CHANNEL  CM_BOTH

#define USE_CONFIG_COLORS
//#define CONFIG_WAITING_CHANNEL         CM_CH2
//#define CONFIG_BLINK_CHANNEL           CM_BOTH

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_BLEND

#define POLICE_COLOR_STROBE_CH1        CM_CH1
#define POLICE_COLOR_STROBE_CH2        CM_CH2

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
#define TINT_RAMPING_CORRECTION 0  // none, linear regulator doesn't need it

// channel 1
//   output: unknown, 6000 lm?
// channel 2
//   output: unknown, 6000 lm?
#define RAMP_SIZE 150
// abstract ramp (power is split between both sets of LEDs)
// 1-130: 0 to 100% power
// level_calc.py 5.01 1 130 7135 2 0.2 2000 --pwm dyn:64:16383:511
// 131 to 150: 101% to 200% power
// level_calc.py 6.44 1 150 7135 1 0.2 2000 --pwm dyn:74:16383:1022
#define PWM1_LEVELS 1,1,1,2,2,2,3,3,4,5,5,6,7,8,9,10,11,12,13,15,16,18,19,21,22,24,26,28,29,31,33,35,37,40,42,44,46,48,50,52,54,56,58,59,61,62,63,64,65,66,66,66,66,65,64,62,60,58,54,50,46,41,35,28,20,21,22,24,25,26,27,29,30,32,33,35,37,38,40,42,44,46,48,50,53,55,57,60,63,65,68,71,74,77,80,83,87,90,94,98,102,106,110,114,118,123,128,132,137,142,148,153,159,164,170,176,183,189,196,202,209,216,224,231,239,247,255,263,272,281,290,299,309,318,328,339,349,360,371,382,394,406,418,430,443,456,469,483,497,511
#define PWM_TOPS 16383,13650,10715,15388,11902,8195,12771,9834,12258,13423,11192,11947,12284,12363,12271,12064,11775,11428,11039,11469,10973,11132,10595,10601,10054,9971,9833,9655,9122,8923,8704,8473,8232,8196,7932,7668,7408,7152,6901,6656,6417,6186,5961,5647,5444,5163,4899,4653,4422,4206,3941,3697,3470,3210,2971,2707,2466,2245,1968,1717,1489,1251,1005,756,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511
// max "200% power" ramp and tops
//#define PWM2_LEVELS 2,2,2,3,3,4,4,5,6,7,8,9,10,11,13,14,16,17,19,21,23,25,28,30,33,35,38,41,44,47,50,54,57,60,64,67,71,74,78,81,84,88,91,94,97,99,101,103,105,106,107,107,107,106,105,102,99,95,90,84,77,68,58,47,34,36,38,40,42,44,47,49,52,54,57,60,63,66,69,73,76,80,83,87,91,96,100,104,109,114,119,124,130,135,141,147,153,160,166,173,180,187,195,203,211,219,228,236,245,255,264,274,285,295,306,317,329,340,353,365,378,391,405,419,433,448,463,479,495,511,530,550,570,591,612,634,657,680,705,730,755,782,809,837,865,895,925,957,989,1022
//#define PWM3_LEVELS 16383,13234,9781,13826,9593,13434,9973,12021,12900,13193,13150,12899,12508,12023,12666,11982,12181,11422,11393,11247,11018,10731,10826,10434,10365,9927,9767,9565,9332,9076,8806,8693,8395,8096,7928,7626,7439,7143,6948,6665,6393,6203,5946,5700,5465,5187,4926,4681,4451,4195,3957,3700,3463,3213,2983,2718,2476,2231,1986,1742,1501,1245,997,756,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511
#define DEFAULT_LEVEL 70
#define MAX_1x7135 150
#define HALFSPEED_LEVEL 10
#define QUARTERSPEED_LEVEL 2

#define RAMP_SMOOTH_FLOOR 10  // level 1 is unreliable (?)
#define RAMP_SMOOTH_CEIL  130
// 10, 30, 50, [70], 90, 110, [130]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit highest regulated power (no FET or turbo)
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS 5

// Allow 3C (or 6C) in Simple UI (toggle smooth or stepped ramping)
#define USE_SIMPLE_UI_RAMPING_TOGGLE

// allow Aux Config and Strobe Modes in Simple UI
#define USE_EXTENDED_SIMPLE_UI

// stop panicking at ~???? lm
#define THERM_FASTER_LEVEL 130
#define MIN_THERM_STEPDOWN 65  // should be above highest dyn_pwm level

// enable 2 click turbo (Anduril 1 style)
#define DEFAULT_2C_STYLE 1

#define USE_POLICE_COLOR_STROBE_MODE

// use the brightest setting for strobe
#define STROBE_BRIGHTNESS MAX_LEVEL
// slow down party strobe; this driver takes ~15ms to start making any light
#define PARTY_STROBE_ONTIME 16

//// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 33

// the power regulator is a bit slow, so push it harder for a quick response from off
//#define DEFAULT_JUMP_START_LEVEL 50
//#define JUMP_START_TIME 50
#define BLINK_BRIGHTNESS 30
#define BLINK_ONCE_TIME 16  // longer blink, since the boost driver is slow

#define THERM_CAL_OFFSET 5

#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// for consistency with KR4 (not otherwise necessary though)
#define USE_SOFT_FACTORY_RESET

