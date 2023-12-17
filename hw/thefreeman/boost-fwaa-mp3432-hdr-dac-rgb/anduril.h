// thefreeman's BCK-FWAA-MP3432 (li-ion / AA)
// Copyright (C) 2023 TBD (thefreeman), Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "thefreeman/boost-fwaa-mp3432-hdr-dac-rgb/hwdef.h"

// HPRsense : 4.2+0.3+20 = 24.5mR (DMN1004UFDF+trace resistance+20mR)
// R1=165k Vsense=49.02 Iout=2001mA
// LPRsense : 2R
// transition DAC lvl 14, ramp lvl 51
// fifth power ramp 0.06mA to 2001mA

#define RAMP_SIZE 150

// 4 ramp segments:
//   - low 0.55V
//   - low 2.5V
//   - high 0.55V
//   - high 2.5V
// PWM1: DAC Data
#define PWM1_LEVELS \
           2,  3,  4,  5,  6,  8,  9, 11, 14, 16, 19, 23, 26, 31, 35, 41, 47, 54, 61, 69, 78, 89,100,112,125,140,155,173,191,212,234, \
          56, 62, 68, 74, 82, 89, 97,106,115,125,136,147,159,172,186,200,215,232,249, \
          14, 15, 17, 18, 19, 20, 22, 23, 25, 26, 28, 30, 32, 34, 36, 38, 40, 43, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81, 86, 90, 95, 99,104,109,114,120,126,131,138,144,150,157,164,171,179,187,195,203,212,221,230,239,249, \
          57, 59, 61, 64, 66, 69, 72, 74, 77, 80, 83, 86, 90, 93, 96,100,103,107,111,115,119,123,127,132,136,141,145,150,155,160,166,171,176,182,188,194,200,206,213,219,226,233,240,247,255
// PWM2: VREF selector (0.55V=16,1.1V=17, 2.5V=18, 4.3V=19, 1.5V=20)
#define PWM2_LEVELS \
        V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05, \
        V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25, \
        V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05, \
        V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25

#define MAX_1x7135           50
#define DEFAULT_LEVEL        44
#define HDR_ENABLE_LEVEL_MIN 51 // when HDR FET turns ON

// no PWM, so MCU clock speed can be slow
#define HALFSPEED_LEVEL      46
#define QUARTERSPEED_LEVEL   45  // seems to run fine at 10kHz/4, try reducing more

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     130  // ~50% power, ~??? mA / ??? lm
// 1 22 [44] 65 87 108 130
#define RAMP_DISCRETE_FLOOR  1
#define RAMP_DISCRETE_CEIL   130
#define RAMP_DISCRETE_STEPS  7

// 20 [45] 70 95 120
#define SIMPLE_UI_FLOOR      20
#define SIMPLE_UI_CEIL       120  // ~37% power, ~??? mA / ??? lm
#define SIMPLE_UI_STEPS      5

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// thermal config

// temperature limit
#define THERM_FASTER_LEVEL  130
#define MIN_THERM_STEPDOWN  MAX_1x7135

//#define THERM_LOOKAHEAD 2 // 4 by default -> decrease for longer turbo
//#define THERM_NEXT_WARNING_THRESHOLD 48 // 24 by default -> increase for fewer adjustments (more stable output on large time scale)
//#define THERM_RESPONSE_MAGNITUDE 16 // 64 by default -> decrease for smaller adjustments (removes dip post turbo)
//#define THERM_WINDOW_SIZE 1 // 2 by default -> decrease for tighter temperature regulation


// UI

//#define SIMPLE_UI_ACTIVE 0 // advanced UI by default

// allow Aux Config and Strobe Modes in Simple UI
//#define USE_EXTENDED_SIMPLE_UI

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE

#define DEFAULT_2C_STYLE 1 // enable 2 click turbo


// AUX

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default
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
#define CANDLE_AMPLITUDE 40

// enable 13H factory reset so it can be used on tail e-switch lights
#define USE_SOFT_FACTORY_RESET

