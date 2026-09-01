// Fireflies Lume1-6af config options for Anduril
// Copyright (C) 2017-2026 Selene ToyKeeper
//               2021-2023 Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// For flashlights using the Loneoceans Lume1-6AF driver (ATTINY1616)
// - NOTE: some flashlights with single or smaller emitters require the no-fet variant of firmware!
// - Models include: E07X Pro, E12R, X1S, X1L, NOV-Mu, T1R, T9R

#define HWDEF_H  fireflies/lume1/6af/hwdef.h
#include "fireflies/anduril.h"


//***************************************
//**    RAMP TABLE AND OPERATIONS      **
//***************************************

#define RAMP_SIZE 150

// Note: The DD FET on this light CANNOT do PWM.
// It's either turbo or nothing.

// levels 1 to 5:
//   ./bin/level_calc.py 5.2 1 5 7135 150 0.01 0.1 --pwm 2500
//   150,403,826,1493,2500
//   (scaled by `x*255/550` or `x*255/2500` to fit)
// levels 6 to 149:
//   ./bin/dac-scale.py 255 255 $( ./bin/level_calc.py 5.96 1 144 7135 26 0.5 3000 --pwm 230000 | grep PWM1 | cut -d : -f 2- )
// level 150:  DD FET only (it can't do PWM, pin is raised and lowered manually)
// top level for each "gear": 2 5 30 50 107 149
#define PWM1_LEVELS \
         69, 186,  \
         84, 152, 255,  \
         12,  14,  17,  20,  24,  28,  32,  38,  43,  49,  56,  63,  70,  79,  89,  99, 110, 122, 135, 150, 165, 182, 200, 220, 241, \
         58,  63,  69,  75,  81,  89,  96, 104, 113, 122, 132, 143, 154, 166, 178, 192, 206, 221, 237, 254, \
         13,  14,  15,  16,  17,  18,  19,  21,  22,  24,  25,  27,  28,  30,  32,  34,  36,  38,  40,  43,  45,  48,  50,  53,  56,  59,  62,  66,  69,  73,  77,  81,  85,  89,  94,  98, 103, 108, 114, 119, 125, 131, 137, 143, 150, 157, 164, 172, 179, 187, 196, 204, 213, 222, 232, 242, 252, \
         57,  60,  62,  65,  68,  70,  73,  76,  79,  82,  86,  89,  92,  96, 100, 104, 108, 112, 116, 120, 124, 129, 134, 139, 144, 149, 154, 160, 165, 171, 177, 183, 189, 196, 202, 209, 216, 224, 231, 239, 246, 255,  \
          0
#define PWM2_LEVELS \
        V05, V05,  \
        V25, V25, V25,  \
        V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, \
        V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
        V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, \
        V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25,  \
          0

// RGB aux ramp
// ./bin/level_calc.py 1.6 1 150 7135 2 0.01 60 --pwm 255
#define PWM3_LEVELS \
        2,2,3,3,3,4,4,5,5,6,6,7,7,8,9,9,10,11,12,12,13,14,15,16,17,18,19,19,20,21,22,24,25,26,27,28,29,30,31,33,34,35,36,38,39,40,42,43,44,46,47,48,50,51,53,54,56,57,59,60,62,63,65,67,68,70,72,73,75,77,78,80,82,84,85,87,89,91,93,94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,143,145,147,149,151,154,156,158,160,163,165,167,170,172,174,177,179,181,184,186,188,191,193,196,198,201,203,206,208,211,213,216,218,221,223,226,228,231,234,236,239,242,244,247,250,252,255

#define LED_PATH1_PIN_LEVEL_MIN   1
#define LED_PATH2_PIN_LEVEL_MIN   6
#define LED_PATH3_PIN_LEVEL_MIN  51

#define HALFSPEED_LEVEL     20
#define QUARTERSPEED_LEVEL  10

#define DEFAULT_LEVEL  70
#define MAX_1x7135     50

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     130

#define RAMP_DISCRETE_FLOOR  10
#define RAMP_DISCRETE_CEIL   130
#define RAMP_DISCRETE_STEPS  7

// 10 40 [70] 100 130
#define SIMPLE_UI_FLOOR      10
#define SIMPLE_UI_CEIL       130
#define SIMPLE_UI_STEPS      5

// don't blink mid-ramp
#undef BLINK_AT_RAMP_MIDDLE
// turbo blink is weird because turbo is weird on this light
#undef BLINK_AT_RAMP_CEIL

//***************************************
//**       THERMAL SETTINGS            **
//***************************************

// set thermal ceiling

#define DEFAULT_THERM_CEIL 50

#define THERM_FASTER_LEVEL  130
#define MIN_THERM_STEPDOWN  70

// thermal tweaking by loneoceans, unsure if necessary
//#define THERM_NEXT_WARNING_THRESHOLD 28 // 24 by default -> increase for fewer adjustments (more stable output on large time scale)
//#define THERM_RESPONSE_MAGNITUDE 64     // 64 by default -> decrease for smaller adjustments (removes dip post turbo)


//***************************************
//**          USER INTERFACE           **
//***************************************

#define BLINK_BRIGHTNESS 30
#define BLINK_ONCE_TIME 15

//***************************************
//**       AUX LEDs and MISC           **
//***************************************

// this light has three aux LED channels: R, G, B
// (and some builds tie these also to a RGB side button)
#define USE_AUX_THRESHOLD_CONFIG
#define USE_SMOOTH_POVD
#define USE_AUXRGB_LEDS_WHILE_ON  25
// don't light up front aux while on, by default
#define DEFAULT_AUX_WHILE_ON  0b01

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN

// this light only has one main led channel
// use aux red + aux blue for police strobe
#define USE_POLICE_COLOR_STROBE_MODE
#define POLICE_STROBE_USES_AUX
#define POLICE_COLOR_STROBE_CH1        CM_AUXRED
#define POLICE_COLOR_STROBE_CH2        CM_AUXBLU


// Misc

#define PARTY_STROBE_ONTIME 1  // slow down party strobe
#define STROBE_OFF_LEVEL 1  // keep the regulator chip on between pulses

// smoother candle mode with bigger oscillations
#define CANDLE_AMPLITUDE 30  // default 25

// attiny1616 has enough space to smooth out voltage readings
#define USE_VOLTAGE_LOWPASS

// enable long-blink as negative sign
#define USE_LONG_BLINK_FOR_NEGATIVE_SIGN

