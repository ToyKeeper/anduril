// BLF GT config options for Anduril
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "lumintop/blf-gt/hwdef.h"

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING

#define RAMP_SIZE 150

// First 60 values: level_calc.py 1 60 7135 4 5.0 255
// Remainder: all 255 (buck driver at 100% duty cycle)
#define PWM1_LEVELS 4,5,6,6,7,8,9,11,12,13,15,16,18,19,21,23,25,27,30,32,34,37,40,43,46,49,52,55,59,63,66,70,75,79,83,88,93,98,103,108,114,119,125,131,137,144,150,157,164,171,179,186,194,202,210,219,228,236,246,255, \
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
// First 60 values: all 25 (buck driver at 10% power)
// Remainder: values 61-150 of level_calc.py 1 150 7135 1 3 3000
#define PWM2_LEVELS 25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25, \
    26,27,28,29,30,31,32,33,35,36,37,38,40,41,42,44,45,47,48,50,51,53,54,56,58,59,61,63,65,67,69,70,72,74,76,79,81,83,85,87,89,92,94,96,99,101,104,106,109,112,114,117,120,123,125,128,131,134,137,140,143,147,150,153,156,160,163,167,170,174,177,181,184,188,192,196,200,204,208,212,216,220,224,228,233,237,241,246,250,255
#define POWER_80PX 138  // 2.0 Amps out of maximum 2.5 Amps
#define MAX_1x7135 60   // where it switches from PWM to current control
#define DEFAULT_LEVEL 69  // nice
#define HALFSPEED_LEVEL 17
#define QUARTERSPEED_LEVEL 6

// use 2.0 A as the ceiling, 2.5 A only for turbo
// start both ramps at the bottom; even moon throws a long way on the GT
#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  POWER_80PX
// 1 23 46 [69] 92 115 138
#define RAMP_DISCRETE_FLOOR 1
#define RAMP_DISCRETE_CEIL  POWER_80PX
#define RAMP_DISCRETE_STEPS 7

// GT can handle heat well, so don't limit simple mode
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS 5

// smoother, more wobbly candle
#define CANDLE_AMPLITUDE 33

// turbo (i.e. "giggles" mode), low, tactical strobe
#define TACTICAL_LEVELS 150,30,(RAMP_SIZE+2)

// stop panicking at 80% power, this light has plenty of thermal mass
#define THERM_FASTER_LEVEL POWER_80PX  // throttle back faster when high

// don't blink during ramp, it's irrelevant and annoying on this light
#undef BLINK_AT_RAMP_CEIL
#undef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_FLOOR

#define USE_SMOOTH_STEPS

// too big, turn off extra features
//#undef USE_TACTICAL_MODE
#undef USE_SOS_MODE
#undef USE_BEACON_MODE
#undef USE_RAMP_AFTER_MOON_CONFIG
//#undef USE_RAMP_SPEED_CONFIG
//#undef USE_VOLTAGE_CORRECTION
#undef USE_2C_STYLE_CONFIG

