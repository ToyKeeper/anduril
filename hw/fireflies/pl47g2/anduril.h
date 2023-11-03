// Fireflies PL47 G2 config options for Anduril
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fireflies/pl47/hwdef.h"

// the button lights up
#define USE_INDICATOR_LED
// the aux LEDs are in the same place as the main LEDs
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

// off mode: low (1)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)



#define RAMP_SIZE 150

// driver is a FET + 3x7135, ~400 lm at highest regulated level
// ramp copied from Emisar D4S ramp
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,5,6,6,7,8,8,9,10,10,11,12,13,14,15,16,17,18,19,21,22,23,25,26,27,29,31,32,34,36,38,40,42,44,46,49,51,54,56,59,62,65,68,71,74,78,81,85,89,93,97,101,106,110,115,120,125,130,136,141,147,153,160,166,173,180,187,195,202,210,219,227,236,245,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,4,5,7,8,10,11,13,14,16,18,19,21,23,25,27,29,31,34,36,38,41,43,46,48,51,54,57,60,63,66,69,72,76,79,83,87,91,95,99,103,107,112,116,121,126,131,136,141,146,152,158,163,169,175,182,188,195,202,209,216,223,231,239,247,255
#define MAX_1x7135 83
#define HALFSPEED_LEVEL 13
#define QUARTERSPEED_LEVEL 6

// ceiling is level 120/150
#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL 120

// 10, 28, 46, 65, 83, 101, 120  (83 is highest regulated)
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  120
#define RAMP_DISCRETE_STEPS 7

// safe limit ~25% power / ~1000 lm
// 10 34 59 [83] 108
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL 108
#define SIMPLE_UI_STEPS 5

// thermal regulation parameters
#ifdef MIN_THERM_STEPDOWN
#undef MIN_THERM_STEPDOWN  // this should be lower, because 3x7135 instead of 1x7135
#endif
#define MIN_THERM_STEPDOWN 60  // lowest value it'll step down to
// regulate down faster when the FET is active, slower otherwise
#define THERM_FASTER_LEVEL 135  // throttle back faster when high

#ifndef BLINK_AT_RAMP_CEIL
#define BLINK_AT_RAMP_CEIL
#endif

// don't do this
#undef BLINK_AT_RAMP_MIDDLE

#define USE_SMOOTH_STEPS

// too big, turn off extra features
#undef USE_SOS_MODE
#undef USE_TACTICAL_MODE
