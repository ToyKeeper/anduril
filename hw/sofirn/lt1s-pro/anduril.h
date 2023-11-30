// Sofirn LT1S Pro config file for Anduril
// Copyright (C) 2022-2023 (FIXME)
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "sofirn/lt1s-pro/hwdef.h"

// off mode: low (1)
// lockout: blinking (3)
// Standby power usage:
// - aux high: 6.9 mA (30 days)
// - aux low:  0.16 mA (3.5 years)
// - red moon: 2.17 mA (96 days)
// - white moon: 1.47 mA (141 days)
// Low mode isn't bright enough to be useful on this light,
// but at least it doesn't drain the battery 3X faster than moon mode.
// (it seriously would be more practical to just use moon instead)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)

// channel modes...
// CM_WHITE, CM_AUTO2, CM_AUTO3, CM_RED, CM_WHITE_RED
#define DEFAULT_CHANNEL_MODE           CM_AUTO3

#define FACTORY_RESET_WARN_CHANNEL     CM_RED
#define FACTORY_RESET_SUCCESS_CHANNEL  CM_WHITE

#define CONFIG_WAITING_CHANNEL         CM_RED
#define CONFIG_BLINK_CHANNEL           CM_WHITE

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_RED

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
// seems unnecessary on this light
#define TINT_RAMPING_CORRECTION 0

#define RAMP_SIZE 150
// use dynamic PWM instead of plain 8-bit
// (so we can get lower lows and a smoother ramp)
// (also, red LEDs use a QX7138 chip which has max PWM speed of 10 kHz,
//  and it behaves erratically at full speed,
//  so PWM here is 576 clock cycles long to keep the speed low enough)
//
// This first ramp seems a bit too low: 0.2 / 1.9 / 10 / 37 / 109 / 272 / 600 lm
// level_calc.py 5.99 1 150 7135 1 0.2 600 --pwm dyn:77:16383:575:3
//#define PWM_LEVELS 1,1,2,2,3,4,4,5,6,6,7,8,9,9,10,11,11,12,13,13,14,15,15,16,16,17,18,18,19,19,19,20,20,21,21,21,21,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,23,23,23,23,24,24,24,25,26,26,27,28,29,30,32,33,34,36,38,40,42,44,46,48,50,52,55,57,59,62,65,68,70,73,77,80,83,86,90,94,97,101,105,110,114,118,123,128,133,138,143,148,154,160,166,172,178,185,191,198,205,213,220,228,236,244,252,261,270,279,289,298,308,319,329,340,351,363,374,386,399,411,424,438,452,466,480,495,510,526,542,558,575
//#define PWM_TOPS 16383,10869,13246,8043,11458,12772,10093,11043,11450,9664,9991,10091,10048,8868,8838,8730,7814,7724,7589,6864,6748,6604,6024,5899,5398,5287,5159,4754,4638,4287,3963,3876,3594,3511,3265,3038,2829,2770,2586,2417,2260,2115,1981,1857,1742,1636,1537,1445,1360,1281,1207,1138,1073,1013,957,904,855,848,803,760,720,714,677,643,637,630,599,592,585,577,569,579,570,560,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575
//
// This ramp is a bit higher: 0.2 / 3 / 19 / 61 / 152 / 320 / 600 lm
// level_calc.py 4.001 1 150 7135 1 0.2 600 --pwm dyn:78:16383:575:3.333
#define PWM1_LEVELS 1,1,2,4,5,6,7,8,9,10,12,13,14,15,17,18,19,21,22,23,24,25,26,27,28,29,30,31,32,33,33,34,34,35,35,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,37,37,38,38,39,40,40,41,43,44,45,47,48,50,52,54,56,59,62,64,67,69,72,75,78,81,84,87,90,94,97,101,104,108,112,116,120,124,128,133,137,142,147,151,156,161,167,172,177,183,189,194,200,206,213,219,226,232,239,246,253,260,268,275,283,291,299,307,316,324,333,342,351,361,370,380,390,400,410,420,431,442,453,464,476,487,499,511,523,536,549,562,575
#define PWM_TOPS 16383,8174,7823,14429,13603,12806,12046,11328,10652,10017,10402,9742,9134,8575,8615,8089,7605,7536,7093,6684,6307,5959,5636,5337,5060,4802,4562,4337,4127,3929,3633,3468,3216,3077,2862,2744,2559,2390,2234,2091,1960,1838,1727,1623,1528,1439,1357,1280,1209,1143,1081,1024,970,919,872,828,787,770,732,716,682,668,654,624,611,613,600,587,587,574,573,571,569,566,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575
// TODO? 200% power at top of ramp on white blend mode
// 2nd table handles "200% power" turbo
//#define PWM2_LEVELS ...
// tops for PWM2
//#define PWM3_LEVELS ...
#define MAX_1x7135 75
#define MIN_THERM_STEPDOWN 75  // should be above highest dyn_pwm level
#define HALFSPEED_LEVEL 12
#define QUARTERSPEED_LEVEL 5

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 40

// override default ramp style
#undef RAMP_STYLE
#define RAMP_STYLE 1  // 0 = smooth, 1 = stepped
// set floor and ceiling as far apart as possible
// because this lantern isn't overpowered
#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     150
//#define RAMP_DISCRETE_FLOOR 17  // 17 50 83 116 150
#define RAMP_DISCRETE_FLOOR  1  // 1 25 50 75 100 125 150
#define RAMP_DISCRETE_CEIL   150
#define RAMP_DISCRETE_STEPS  7

// LT1S can handle heat well, so don't limit simple mode
//#define SIMPLE_UI_FLOOR 10  // 10 45 80 115 150
#define SIMPLE_UI_FLOOR  1
#define SIMPLE_UI_CEIL   150
#define SIMPLE_UI_STEPS  7

// Allow 3C (or 6C) in Simple UI (toggle smooth or stepped ramping)
#define USE_SIMPLE_UI_RAMPING_TOGGLE

// allow Aux Config and Strobe Modes in Simple UI
//#define USE_EXTENDED_SIMPLE_UI

// turn on at med-low brightness by default (level 50/150, or ramp step 3/7)
// (also sets lockout mode 2H to a useful level)
//#define DEFAULT_MANUAL_MEMORY 50
// reset to default after being off for 10 minutes
//#define DEFAULT_MANUAL_MEMORY_TIMER 10

// enable 2 click turbo (Anduril 1 style)
#define DEFAULT_2C_STYLE 1

#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

#define USE_POLICE_COLOR_STROBE_MODE
#define POLICE_COLOR_STROBE_CH1        CM_RED
#define POLICE_COLOR_STROBE_CH2        CM_WHITE

#undef  TACTICAL_LEVELS
#define TACTICAL_LEVELS 120,30,(RAMP_SIZE+3)  // high, low, police strobe

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif
#ifdef BLINK_AT_RAMP_FLOOR
#undef BLINK_AT_RAMP_FLOOR
#endif
#ifdef BLINK_AT_RAMP_CEIL
#undef BLINK_AT_RAMP_CEIL
#endif
// without this, it's really hard to tell when ramping up stops
#define BLINK_AT_RAMP_CEIL

#define USE_SOFT_FACTORY_RESET
