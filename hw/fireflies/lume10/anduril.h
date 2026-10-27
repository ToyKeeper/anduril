// Firefly Lume10 config options for Anduril
// Copyright (C) 2017-2026 Selene ToyKeeper
//               2022-2024 Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// For flashlights using the Loneoceans Lume10 buck+FET driver (AVR32DD20)

#define HWDEF_H  fireflies/lume10/hwdef.h
#define USE_AW2016
#include "fireflies/anduril.h"


//***************************************
//**    RAMP TABLE AND OPERATIONS      **
//***************************************

#define RAMP_SIZE 150

// Note: The DD FET on this light CANNOT do PWM.
// It's either turbo or nothing.

// levels 1 to 5:
//   ./bin/level_calc.py 5.2 1 5 7135 100 0.01 0.1 --pwm 2500
//   (with the last 2 values divided to fit)
// levels 6 to 149: (150 is DD FET only)
//   ./bin/dac-scale.py $( ./bin/level_calc.py 5.79 1 144 7135 18 0.2 3000 --pwm 293000 | grep PWM1 | cut -d : -f 2- )
//   ./bin/dac-scale.py $( ./bin/level_calc.py 6.18 1 144 7135 18 0.2 3000 --pwm 330000 | grep PWM1 | cut -d : -f 2- )
// top level for each "gear": 3 5 / 38 50 / 124 149
#define PWM1_LEVELS \
         100, 359, 790, 588,1023, \
          17,  21,  25,  30,  36,  42,  49,  58,  67,  77,  88, 100, 114, 130, 147, 166, 186, 209, 234, 262, 292, 325, 361, 400, 442, 488, 538, 593, 652, 715, 784, 858, 938, \
         419, 457, 497, 541, 588, 638, 692, 750, 811, 876, 946,1020, \
          20,  21,  23,  25,  27,  29,  31,  33,  35,  38,  41,  43,  46,  49,  53,  56,  60,  64,  68,  72,  77,  81,  86,  92,  97, 103, 109, 115, 122, 129, 136, 144, 152, 160, 169, 178, 187, 197, 208, 218, 230, 242, 254, 267, 280, 294, 308, 324, 339, 356, 373, 390, 408, 428, 447, 468, 489, 511, 534, 558, 583, 609, 635, 663, 691, 721, 751, 783, 816, 850, 885, 921, 959, 998, \
         425, 442, 459, 477, 496, 515, 535, 556, 577, 599, 622, 645, 669, 694, 720, 746, 773, 801, 830, 860, 891, 922, 955, 988,1023, \
        1023
#define PWM2_LEVELS \
         V10, V10, V10, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
         V25
// RGB aux ramp
// ./bin/level_calc.py 1.6 1 150 7135 2 0.01 60 --pwm 255
#define PWM3_LEVELS \
        2,2,3,3,3,4,4,5,5,6,6,7,7,8,9,9,10,11,12,12,13,14,15,16,17,18,19,19,20,21,22,24,25,26,27,28,29,30,31,33,34,35,36,38,39,40,42,43,44,46,47,48,50,51,53,54,56,57,59,60,62,63,65,67,68,70,72,73,75,77,78,80,82,84,85,87,89,91,93,94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,143,145,147,149,151,154,156,158,160,163,165,167,170,172,174,177,179,181,184,186,188,191,193,196,198,201,203,206,208,211,213,216,218,221,223,226,228,231,234,236,239,242,244,247,250,252,255

#define LED_PATH1_PIN_LEVEL_MIN   1
#define LED_PATH2_PIN_LEVEL_MIN   6
#define LED_PATH3_PIN_LEVEL_MIN  51
#define LED_DDFET_LEVEL_MIN     150  // unused, just a reminder that lvl 150 is DD FET

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
//#undef BLINK_AT_RAMP_CEIL

//***************************************
//**       THERMAL SETTINGS            **
//***************************************

#define THERM_FASTER_LEVEL  100
#define MIN_THERM_STEPDOWN  60

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

// has a powerbank host function
#define USE_POWERBANK_HOST_MODE

