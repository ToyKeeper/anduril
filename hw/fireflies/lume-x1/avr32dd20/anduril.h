// Fireflies Lume-X1 (avr32dd20, 40W) config options for Anduril
// Copyright (C) 2017-2026 Selene ToyKeeper
//               2022-2024 Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// For flashlights using the Loneoceans Lume-X1-40W boost driver (AVR32DD20)
// - Same firmware for 6V, 9V, or 12V configs

#define HWDEF_H  fireflies/lume-x1/avr32dd20/hwdef.h
#include "fireflies/anduril.h"


//***************************************
//**    RAMP TABLE AND OPERATIONS      **
//***************************************

#define RAMP_SIZE 150

// maximum 83.6% power for 40W turbo
// levels 1 to 5:
//   ./bin/level_calc.py 5.2 1 5 7135 100 0.01 0.1 --pwm 2500
//   (with the last 2 values divided to fit)
// levels 6 to 150:
//   ./bin/dac-scale.py 856 1023 $( ./bin/level_calc.py 5.07 1 145 7135 21 0.2 3000 --pwm 275000 | grep PWM1 | cut -d : -f 2- )
// top level for each "gear": 3 5 / 38 50 / 127 150
#define PWM1_LEVELS \
         100, 359, 790, 588,1023, \
          17,  20,  25,  29,  34,  40,  47,  55,  64,  73,  85,  97, 111, 127, 144, 164, 184, 208, 234, 262, 293, 327, 364, 404, 448, 496, 548, 604, 664, 729, 799, 876, 957, \
         427, 465, 507, 550, 598, 647, 701, 758, 818, 883, 951,1023, \
          24,  26,  28,  30,  32,  34,  36,  39,  42,  44,  47,  50,  54,  57,  61,  65,  68,  73,  77,  81,  86,  91,  96, 102, 108, 113, 120, 126, 133, 140, 147, 155, 163, 171, 179, 188, 198, 207, 217, 228, 238, 249, 261, 273, 286, 298, 312, 326, 340, 355, 370, 386, 402, 419, 437, 455, 474, 493, 513, 534, 555, 577, 599, 623, 647, 671, 697, 723, 750, 778, 807, 837, 867, 898, 931, 964, 998, \
         423, 438, 453, 468, 484, 501, 518, 535, 553, 571, 590, 609, 629, 649, 670, 691, 713, 736, 759, 782, 806, 831, 856
#define PWM2_LEVELS \
         V10, V10, V10, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25

// RGB aux ramp
// ./bin/level_calc.py 1.6 1 150 7135 2 0.01 60 --pwm 255
#define PWM3_LEVELS \
        2,2,3,3,3,4,4,5,5,6,6,7,7,8,9,9,10,11,12,12,13,14,15,16,17,18,19,19,20,21,22,24,25,26,27,28,29,30,31,33,34,35,36,38,39,40,42,43,44,46,47,48,50,51,53,54,56,57,59,60,62,63,65,67,68,70,72,73,75,77,78,80,82,84,85,87,89,91,93,94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,143,145,147,149,151,154,156,158,160,163,165,167,170,172,174,177,179,181,184,186,188,191,193,196,198,201,203,206,208,211,213,216,218,221,223,226,228,231,234,236,239,242,244,247,250,252,255

#define LED_PATH1_PIN_LEVEL_MIN   1
#define LED_PATH2_PIN_LEVEL_MIN   6
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

#define AVRXMEGA3   // used in factory-reset.c for thermal offset cal, probably should refactor

// set thermal ceiling

#define DEFAULT_THERM_CEIL 50

// stop panicking at 6W (not sure of this numbers yet since it depends on the host..)
#define THERM_FASTER_LEVEL  100  // about 6W
#define MIN_THERM_STEPDOWN  60   // similar to single amc7135 in a 3V light


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

// avr32dd20 has enough space to smooth out voltage readings
#define USE_VOLTAGE_LOWPASS

// enable long-blink as negative sign
#define USE_LONG_BLINK_FOR_NEGATIVE_SIGN

