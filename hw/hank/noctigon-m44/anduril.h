// Noctigon M44 config options for Anduril
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "hank/noctigon-m44/hwdef.h"
#include "hank/anduril.h"

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
#define DEFAULT_BLINK_CHANNEL  CM_BOTH

#define POLICE_COLOR_STROBE_CH1        CM_CH1
#define POLICE_COLOR_STROBE_CH2        CM_CH2
// aux red + aux blue are the correct colors, but are dim
//#define POLICE_COLOR_STROBE_CH1        CM_AUXRED
//#define POLICE_COLOR_STROBE_CH2        CM_AUXBLU

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
#define TINT_RAMPING_CORRECTION 0  // none, linear regulator doesn't need it

// channel 1
//   output: unknown, 6000 lm?
// channel 2
//   output: unknown, 6000 lm?
#define RAMP_SIZE 150
#if 0  // optimized hand-tweaked ramp from before PWM+DSM update
// "100% power" ramp
// level_calc.py 5.01 1 150 7135 0 2.0 5000 --pwm dyn:64:16384:511:5
// (with manual tweaks)
#define PWM1_LEVELS 0,1,1,2,2,3,4,5,6,6,7,7,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,9,9,9,9,9,10,10,11,11,12,12,13,14,15,16,17,18,19,20,21,22,23,24,26,27,28,29,31,32,34,35,37,39,40,42,44,46,48,50,52,54,57,59,62,64,67,69,72,75,78,81,84,87,91,94,98,101,105,109,113,117,121,126,130,135,140,145,150,155,160,165,171,177,183,189,195,201,208,214,221,228,236,243,251,258,266,274,283,291,300,309,318,328,337,347,357,368,378,389,400,411,423,435,447,459,472,484,498,511
#define PWM_TOPS 16383,16383,10002,11695,8083,9374,9793,7993,8291,7017,7180,6235,5431,5556,4927,4385,3916,3511,3159,2852,2585,2349,2142,1958,1795,1648,1517,1398,1290,1193,1104,1023,950,883,822,765,714,667,623,582,632,610,588,566,544,512,535,505,523,494,509,480,492,502,509,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511
// max "200% power" ramp and tops
// 1-130: 0 to 100% power
// level_calc.py 5.01 1 130 7135 2 0.2 2000 --pwm dyn:64:16383:511
// 131 to 150: 101% to 200% power
// level_calc.py 6.44 1 150 7135 1 0.2 2000 --pwm dyn:74:16383:1022
//#define PWM2_LEVELS 2,2,2,3,3,4,4,5,6,7,8,9,10,11,13,14,16,17,19,21,23,25,28,30,33,35,38,41,44,47,50,54,57,60,64,67,71,74,78,81,84,88,91,94,97,99,101,103,105,106,107,107,107,106,105,102,99,95,90,84,77,68,58,47,34,36,38,40,42,44,47,49,52,54,57,60,63,66,69,73,76,80,83,87,91,96,100,104,109,114,119,124,130,135,141,147,153,160,166,173,180,187,195,203,211,219,228,236,245,255,264,274,285,295,306,317,329,340,353,365,378,391,405,419,433,448,463,479,495,511,530,550,570,591,612,634,657,680,705,730,755,782,809,837,865,895,925,957,989,1022
//#define PWM3_LEVELS 16383,13234,9781,13826,9593,13434,9973,12021,12900,13193,13150,12899,12508,12023,12666,11982,12181,11422,11393,11247,11018,10731,10826,10434,10365,9927,9767,9565,9332,9076,8806,8693,8395,8096,7928,7626,7439,7143,6948,6665,6393,6203,5946,5700,5465,5187,4926,4681,4451,4195,3957,3700,3463,3213,2983,2718,2476,2231,1986,1742,1501,1245,997,756,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511
#define MIN_THERM_STEPDOWN 65  // should be above highest dyn_pwm level
#define DEFAULT_LEVEL 70
#define MAX_1x7135 150
#define HALFSPEED_LEVEL 10
#define QUARTERSPEED_LEVEL 2
#endif  // end old ramp config

// delta-sigma modulated PWM (0b0HHHHHHHHLLLLLLL = 0, 8xHigh, 7xLow bits)
// level_calc.py 5.01 1 150 7135 0 0.2 2000 --pwm 32640
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
#define PWM1_LEVELS 0,1,2,3,4,5,6,7,9,10,12,14,17,19,22,25,28,32,36,41,45,50,56,62,69,76,84,92,101,110,121,132,143,156,169,184,199,215,232,251,270,291,313,336,360,386,414,442,473,505,539,574,612,651,693,736,782,829,880,932,987,1045,1105,1168,1233,1302,1374,1449,1527,1608,1693,1781,1873,1969,2068,2172,2279,2391,2507,2628,2753,2883,3018,3158,3303,3454,3609,3771,3938,4111,4289,4475,4666,4864,5068,5280,5498,5724,5957,6197,6445,6701,6965,7237,7518,7808,8106,8413,8730,9056,9392,9737,10093,10459,10835,11223,11621,12031,12452,12884,13329,13786,14255,14737,15232,15741,16262,16798,17347,17911,18489,19082,19691,20314,20954,21609,22281,22969,23674,24397,25137,25895,26671,27465,28279,29111,29963,30835,31727,32640

#define MIN_THERM_STEPDOWN 50
#define DEFAULT_LEVEL 70
#define MAX_1x7135 150
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

// stop panicking at ~???? lm
#define THERM_FASTER_LEVEL 130

#define USE_POLICE_COLOR_STROBE_MODE
#undef  TACTICAL_LEVELS
#define TACTICAL_LEVELS 120,30,(RAMP_SIZE+3)  // high, low, police strobe

// use the brightest setting for strobe
#define STROBE_BRIGHTNESS MAX_LEVEL
// slow down party strobe; this driver takes a while to start making any light
#define PARTY_STROBE_ONTIME  12
//#define STROBE_OFF_LEVEL 1  // nope, this makes strobe blurry
// bike strobe needs a longer pulse too?  nope
#define BIKE_STROBE_ONTIME 8

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 33

// party strobe, tac strobe, police, lightning, candle, bike
#define DEFAULT_STROBE_CHANNELS  CM_BOTH,CM_BOTH,CM_BOTH,CM_AUTO,CM_AUTO,CM_AUTO

// the power regulator is a bit slow, so push it harder for a quick response from off
//#define DEFAULT_JUMP_START_LEVEL 50
//#define JUMP_START_TIME 50
#define BLINK_BRIGHTNESS 30
#define BLINK_ONCE_TIME 16  // longer blink, since the boost driver is slow

#define THERM_CAL_OFFSET 5

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// for consistency with KR4 (not otherwise necessary though)
#define USE_SOFT_FACTORY_RESET

