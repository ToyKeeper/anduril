// lume1 for FW3x config options for Anduril
// Copyright (C) 2020-2023 LoneOceans, Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * Constant current Buck-Boost + FET driver
 * For more information: www.loneoceans.com/labs/
 * Datasheets:
 * - 1634: http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8303-8-bit-AVR-Microcontroller-tinyAVR-ATtiny1634_Datasheet.pdf
 */

#define HWDEF_H  lumintop/fw3x-lume1/hwdef.h

// set this light for 50C thermal ceiling
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 50

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// the aux LEDs are front-facing, so turn them off while main LEDs are on
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

#define RAMP_SIZE 150

// level_calc.py 5.01 1 149 7135 0 0.2 2000 --pwm 32640
// regulated channel: 16-bit PWM+DSM for low lows and very smooth ramp
#define PWM1_LEVELS  0,1,2,3,4,5,6,7,9,10,12,14,17,19,22,25,29,33,37,41,46,51,57,63,70,77,85,93,103,112,123,134,146,159,172,187,203,219,237,256,276,297,319,343,368,394,422,452,483,516,551,587,625,666,708,753,799,848,900,954,1010,1069,1131,1195,1263,1333,1407,1483,1563,1647,1734,1825,1919,2017,2119,2226,2336,2451,2571,2694,2823,2957,3095,3239,3388,3542,3702,3868,4040,4217,4401,4591,4788,4992,5202,5419,5644,5876,6115,6362,6617,6880,7152,7432,7721,8018,8325,8641,8967,9302,9647,10003,10369,10745,11133,11531,11941,12363,12796,13241,13699,14169,14652,15148,15657,16180,16717,17268,17834,18414,19009,19620,20246,20888,21546,22221,22913,23621,24348,25091,25853,26634,27433,28251,29089,29946,30823,31721,32640,0
// DD FET: 8-bit PWM (but hardware can only handle 0 and MAX)
#define PWM2_LEVELS  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255
#define DEFAULT_LEVEL 56
#define MAX_1x7135 149
#define MIN_THERM_STEPDOWN 50
#define HALFSPEED_LEVEL 11
#define QUARTERSPEED_LEVEL 2

#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL 149
// 10 33 56 79 102 125 [149]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL 120
#define SIMPLE_UI_STEPS 5

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN

// slow down party strobe; this driver can't pulse for too short a time
#define PARTY_STROBE_ONTIME 1

// use aux red + aux blue for police strobe
#define USE_POLICE_COLOR_STROBE_MODE
#define POLICE_STROBE_USES_AUX
#define POLICE_COLOR_STROBE_CH1        CM_AUXRED
#define POLICE_COLOR_STROBE_CH2        CM_AUXBLU

// stop panicking at ~85% regulated power or ~750 lm
#define THERM_FASTER_LEVEL 140

#define THERM_CAL_OFFSET 0  // not needed due to external sensor

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif


// can't reset the normal way because power is connected before the button
#define USE_SOFT_FACTORY_RESET

