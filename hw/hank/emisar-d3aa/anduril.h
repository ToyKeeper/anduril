// Emisar D3AA config options for Anduril
// Copyright (C) 2023 thefreeman, Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define HWDEF_H hank/emisar-d3aa/hwdef.h
#include "hank/anduril.h"

// HPRsense : 4.2+0.3+20 = 24.5mR
// Vsense=42.46mV, R1= 165k
// LPRsense : 3R3
// HDR ratio: 131.5
// transition DAC level 20, ramp level 48
// fifth power ramp 0.02mA to 2001mA

#define RAMP_SIZE 150

// 4 ramp segments:
//   - low  1.024V
//   - low  2.5  V
//   - high 1.024V
//   - high 2.5  V
// HDR ratio: 131.5
// PWM1: DAC Data
// 131.5 * 1024 * 2.5 = 336640 total dimming ratio
// ./bin/dac-scale.py $( ./bin/level_calc.py 4.106 1 150 7135 3 0.01 1400 --pwm 336640 | grep PWM1 | cut -d : -f 2- )
// top level for each "gear": 30 40 119 150
#define PWM1_LEVELS \
           3,   5,   6,   9,  12,  16,  21,  28,  35,  44,  55,  68,  83, 101, 121, 144, 170, 200, 234, 271, 313, 360, 412, 470, 534, 603, 680, 764, 855, 954, \
         434, 482, 534, 590, 650, 715, 784, 858, 938,1023, \
          20,  21,  23,  25,  27,  30,  32,  35,  37,  40,  43,  46,  49,  53,  56,  60,  64,  68,  73,  77,  82,  87,  93,  98, 104, 110, 116, 123, 129, 137, 144, 152, 160, 168, 177, 185, 195, 204, 214, 225, 235, 247, 258, 270, 282, 295, 308, 322, 336, 350, 365, 381, 396, 413, 430, 447, 465, 484, 503, 522, 543, 563, 585, 607, 629, 653, 677, 701, 726, 752, 779, 806, 834, 863, 892, 923, 954, 985,1018, \
         430, 444, 459, 473, 488, 504, 520, 536, 552, 569, 587, 604, 622, 641, 660, 679, 699, 719, 740, 761, 782, 804, 827, 849, 873, 897, 921, 946, 971, 997,1023
#define PWM2_LEVELS \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25
#define MAX_1x7135            40
#define HDR_ENABLE_LEVEL_MIN  41

#define DEFAULT_LEVEL         50

// no PWM, so MCU clock speed can be slow
#define HALFSPEED_LEVEL      41
#define QUARTERSPEED_LEVEL   40  // seems to run fine at 10kHz/4, try reducing more?

// should be about 300 mA or ~100 lm,
// to avoid overloading firmware flashing adapters
#define WEAK_BATTERY_TEST_MAX_LEVEL       75

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     130  // 50% power
// 10 30 [50] 70 90 110 130
#define RAMP_DISCRETE_FLOOR  10
#define RAMP_DISCRETE_CEIL   130
#define RAMP_DISCRETE_STEPS  7

// 10 [40] 70 100 130
#define SIMPLE_UI_FLOOR      10
#define SIMPLE_UI_CEIL       130
#define SIMPLE_UI_STEPS      5

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// thermal config

// temperature limit
#define THERM_FASTER_LEVEL  130 // stop panicking at 50%/1A
#define MIN_THERM_STEPDOWN  MAX_1x7135


// UI

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE

#define DEFAULT_2C_STYLE 1 // enable 2 click turbo


// AUX

#define USE_BUTTON_LED

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN

// use aux red + aux blue for police strobe
#define USE_POLICE_COLOR_STROBE_MODE
#define POLICE_STROBE_USES_AUX
#define POLICE_COLOR_STROBE_CH1        CM_AUXRED
#define POLICE_COLOR_STROBE_CH2        CM_AUXBLU

// the aux LEDs are front-facing, so turn them off while main LEDs are on
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif


// Misc

#define PARTY_STROBE_ONTIME 1  // slow down party strobe
#define STROBE_OFF_LEVEL 1  // keep the regulator chip on between pulses

// smoother candle mode with bigger oscillations
#define CANDLE_AMPLITUDE 33

// added for convenience
#define USE_SOFT_FACTORY_RESET

