// Fireflies Lume-X1 (attiny1616, 40W) config options for Anduril
// Copyright (C) 2017-2026 Selene ToyKeeper
//               2021-2024 loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// For flashlights using the Loneoceans Lume-X1-40W boost driver (ATTINY1616)
// - Models include: X1S, X1L, etc

#define HWDEF_H  fireflies/lume-x1/attiny1616/hwdef.h
#include "fireflies/anduril.h"


//***************************************
//**    RAMP TABLE AND OPERATIONS      **
//***************************************

#define RAMP_SIZE 150

// levels 1 to 9:
//   ./bin/level_calc.py 5.2 1 9 7135 100 0.01 0.1 --pwm 2500
//   (scaled by `x*255/550` or `x*255/2500` to fit)
// levels 10 to 150:
//   ./bin/dac-scale.py 255 255 $( ./bin/level_calc.py 5.055 1 141 7135 26 0.5 3000 --pwm 218000 | grep PWM1 | cut -d : -f 2- )
// top level for each "gear": 4 9 31 50 105 150
#define PWM1_LEVELS \
        46,  98, 166, 254,  \
        80, 111, 150, 197, 255,  \
        12,  15,  18,  22,  26,  31,  37,  43,  50,  57,  66,  75,  85,  96, 108, 122, 137, 153, 171, 190, 210, 233, \
        56,  62,  68,  75,  82,  90,  98, 107, 117, 127, 138, 149, 162, 175, 189, 204, 220, 237, 255, \
        14,  15,  16,  17,  18,  20,  21,  22,  24,  26,  27,  29,  31,  33,  35,  37,  39,  42,  44,  47,  49,  52,  55,  58,  61,  65,  68,  72,  76,  80,  84,  88,  92,  97, 102, 107, 112, 117, 123, 129, 135, 141, 148, 154, 161, 169, 176, 184, 192, 200, 209, 217, 227, 236, 246, \
        56,  58,  61,  63,  65,  68,  71,  74,  76,  79,  82,  85,  89,  92,  95,  99, 102, 106, 110, 114, 118, 122, 126, 131, 135, 140, 145, 149, 154, 160, 165, 170, 176, 182, 187, 193, 200, 206, 212, 219, 226, 233, 240, 247, 255
#define PWM2_LEVELS \
        V05, V05, V05, V05,  \
        V25, V25, V25, V25, V25,  \
        V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, \
        V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
        V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, \
        V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25

// RGB aux ramp
// ./bin/level_calc.py 1.6 1 150 7135 2 0.01 60 --pwm 255
#define PWM3_LEVELS \
        2,2,3,3,3,4,4,5,5,6,6,7,7,8,9,9,10,11,12,12,13,14,15,16,17,18,19,19,20,21,22,24,25,26,27,28,29,30,31,33,34,35,36,38,39,40,42,43,44,46,47,48,50,51,53,54,56,57,59,60,62,63,65,67,68,70,72,73,75,77,78,80,82,84,85,87,89,91,93,94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,143,145,147,149,151,154,156,158,160,163,165,167,170,172,174,177,179,181,184,186,188,191,193,196,198,201,203,206,208,211,213,216,218,221,223,226,228,231,234,236,239,242,244,247,250,252,255

#define LED_PATH1_PIN_LEVEL_MIN   1
#define LED_PATH2_PIN_LEVEL_MIN  10
#define LED_PATH3_PIN_LEVEL_MIN  51

#define HALFSPEED_LEVEL     20
#define QUARTERSPEED_LEVEL  10

#define DEFAULT_LEVEL  50
#define MAX_1x7135     50

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     130

#define RAMP_DISCRETE_FLOOR  10
#define RAMP_DISCRETE_CEIL   130
#define RAMP_DISCRETE_STEPS  7

// 10 35 [60] 85 110
#define SIMPLE_UI_FLOOR      10
#define SIMPLE_UI_CEIL       110      // about ~12W
#define SIMPLE_UI_STEPS      5

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

//***************************************
//**       THERMAL SETTINGS            **
//***************************************

// set thermal ceiling

#define DEFAULT_THERM_CEIL 50

#define THERM_FASTER_LEVEL  110
#define MIN_THERM_STEPDOWN  50

// thermal tweaking by loneoceans, unsure if necessary
#define THERM_NEXT_WARNING_THRESHOLD  16  // 24 by default -> increase for fewer adjustments (more stable output on large time scale)
#define THERM_RESPONSE_MAGNITUDE      32  // 64 by default -> decrease for smaller adjustments (removes dip post turbo)


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

// has a powerbank host function
#define USE_POWERBANK_HOST_MODE

