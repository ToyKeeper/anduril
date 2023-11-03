// Mateminco MT18S / Astrolux MF01S options for Anduril
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "mateminco/mf01s/hwdef.h"

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
//#define USE_INDICATOR_LED_WHILE_RAMPING
// off mode: low (1)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)


#define RAMP_SIZE 150

// measured brightness with 4x30Q cells at 4.11V:
// moon: 2.5 lm
// channel 1: 617 lm
// channel 2: 13500 lm
// ../../../bin/level_calc.py seventh 2 150 7135 1 12 717 FET 1 10 13000
// (with some manual tweaks afterward)
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,5,6,7,7,8,9,10,11,12,13,14,15,17,18,19,21,22,24,26,28,30,32,34,36,38,41,44,46,49,52,55,59,62,66,70,74,78,83,87,92,97,102,108,114,120,126,133,139,147,154,162,170,178,187,196,206,215,226,236,248,255,235,255,240,255,245,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,3,3,4,4,5,6,7,8,9,10,11,12,13,14,16,17,19,20,22,23,25,26,28,30,31,33,35,37,39,41,43,45,47,49,52,54,57,59,62,65,67,70,73,76,80,83,86,90,93,97,101,105,109,113,117,122,126,131,135,140,145,151,156,161,167,173,179,185,191,197,204,211,218,225,232,239,247,255

#define MAX_1x7135 70  // ~626 lm
#define HALFSPEED_LEVEL 23
#define QUARTERSPEED_LEVEL 6

#define RAMP_SMOOTH_FLOOR 1   // ~2.5 lm
#define RAMP_SMOOTH_CEIL  120  // ~5400 lm
// 20, 36, 53, [70], 86, 103, 120
#define RAMP_DISCRETE_FLOOR 20 // 35 lm
#define RAMP_DISCRETE_CEIL  120 // ~5400 lm
#define RAMP_DISCRETE_STEPS 7  // 35, 108, 280, 626, 1500, 2930, 5400 lm

// safe limit ~25% power
// 18 35 52 [70] 87 105
#define SIMPLE_UI_FLOOR 18
#define SIMPLE_UI_CEIL 105
#define SIMPLE_UI_STEPS 6

#define THERM_FASTER_LEVEL 125  // throttle back faster when high (>6000 lm)
#define THERM_HARD_TURBO_DROP  // this light is massively overpowered

// don't blink during ramp, it's irrelevant and annoying on this light
#define BLINK_AT_RAMP_CEIL
#undef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_FLOOR

// enable extra features
#define USE_SMOOTH_STEPS

// too big, turn off extra features
#undef USE_TACTICAL_MODE
#undef USE_SOS_MODE

