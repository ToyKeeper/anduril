// Fireflies E01 SST-40 thrower config options for Anduril
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fireflies/pl47/hwdef.h"

// disable indicator LED; it's hardwired
#ifdef USE_INDICATOR_LED
#undef USE_INDICATOR_LED
#endif


#define RAMP_SIZE 150

// driver is a FET + 3x7135, ~413 lm at highest regulated level
// ../../../bin/level_calc.py seventh 2 150 7135 1 12 414 FET 2 10 1930
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,5,6,6,7,8,8,9,10,10,11,12,13,14,15,15,16,17,18,20,21,22,23,24,26,27,28,30,31,33,34,36,38,39,41,43,45,47,49,51,53,56,58,60,63,65,68,71,74,77,80,83,86,89,93,96,100,103,107,111,115,119,124,128,132,137,142,147,152,157,163,168,174,180,186,192,198,204,211,218,225,232,240,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,5,7,9,12,14,16,19,22,24,27,30,33,36,39,42,45,48,52,55,58,62,66,69,73,77,81,85,90,94,98,103,107,112,117,122,127,132,137,143,148,154,160,166,172,178,184,191,197,204,211,218,225,232,240,247,255
#define MAX_1x7135 93
#define HALFSPEED_LEVEL 14
#define QUARTERSPEED_LEVEL 7

// ceiling is level 130/150  (50% power)
#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL 130

// 20, 38, 56, 75, [93], 111, 130 (93 is highest regulated)
// (9 / 45 / 116 / 248 / 467 / 742 / 1280 + 2140 lm)
#define RAMP_DISCRETE_FLOOR 20
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit ~50% power
// 20 56 [93] 130
#define SIMPLE_UI_FLOOR 20
#define SIMPLE_UI_CEIL 130
#define SIMPLE_UI_STEPS 4

// thermal regulation parameters
#define MIN_THERM_STEPDOWN 65  // lowest value it'll step down to
#define THERM_FASTER_LEVEL 130

#ifndef BLINK_AT_RAMP_CEIL
#define BLINK_AT_RAMP_CEIL
#endif

// don't do this
#undef BLINK_AT_RAMP_MIDDLE

#define USE_SMOOTH_STEPS

// too big, turn off extra features
#undef USE_SOS_MODE
//#undef USE_TACTICAL_MODE
