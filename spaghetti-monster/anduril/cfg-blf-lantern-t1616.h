// BLF Lantern config options for Anduril using the Attiny1616
// Copyright (C) 2021-2023 (original author TBD), Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define MODEL_NUMBER "0622"
#include "hwdef-blf-lt1-t1616.h"
// ATTINY: 1616

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING
// off mode: low (1)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)

// channel modes...
// CM_CH1, CM_CH2, CM_BOTH, CM_BLEND, CM_AUTO
#define DEFAULT_CHANNEL_MODE  CM_AUTO
#define DEFAULT_BLINK_CHANNEL  CM_BOTH

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
//#define TINT_RAMPING_CORRECTION 26  // prototype, 140%
#define TINT_RAMPING_CORRECTION 10  // production model, 115%

#define RAMP_SIZE 150
// level_calc.py 1 150 7135 1 30 800
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,11,11,12,13,13,14,15,15,16,17,18,18,19,20,21,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,43,44,45,46,48,49,50,51,53,54,56,57,58,60,61,63,64,66,67,69,70,72,74,75,77,79,80,82,84,85,87,89,91,93,95,97,98,100,102,104,106,108,111,113,115,117,119,121,124,126,128,130,133,135,137,140,142,145,147,150,152,155,157,160,163,165,168,171,173,176,179,182,185,188,190,193,196,199,202,205,209,212,215,218,221,224,228,231,234,238,241,245,248,251,255
#define MAX_1x7135 65
#define HALFSPEED_LEVEL 14
#define QUARTERSPEED_LEVEL 5

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 40

// override default ramp style
#undef RAMP_STYLE
#define RAMP_STYLE  1  // 0 = smooth, 1 = stepped
// set floor and ceiling as far apart as possible
// because this lantern isn't overpowered
#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     150
#define RAMP_DISCRETE_FLOOR  10
#define RAMP_DISCRETE_CEIL   150
#define RAMP_DISCRETE_STEPS  7

// LT1 can handle heat well, so don't limit simple mode
#define SIMPLE_UI_FLOOR  10
#define SIMPLE_UI_CEIL   150
#define SIMPLE_UI_STEPS  5

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE
#define USE_EXTENDED_SIMPLE_UI

#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

// the sensor (attiny1616) is nowhere near the emitters
// so thermal regulation can't work
#ifdef USE_THERMAL_REGULATION
#undef USE_THERMAL_REGULATION
#endif

// don't blink while ramping
#ifdef BLINK_AT_RAMP_FLOOR
#undef BLINK_AT_RAMP_FLOOR
#endif
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif
// except the top... blink at the top
#ifndef BLINK_AT_RAMP_CEIL
#define BLINK_AT_RAMP_CEIL
#endif

#define USE_CHANNEL_MODE_ARGS
#define USE_SET_LEVEL_GRADUALLY

// for consistency with other models
#define USE_SOFT_FACTORY_RESET

