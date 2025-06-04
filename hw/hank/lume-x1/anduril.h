// Hank Emisar/Noctigon Lume-X1 config options for Anduril
// Copyright (C) 2018-2024 Selene ToyKeeper, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// For flashlights using the Loneoceans Lume-X1-40W boost driver (AVR32DD20)
// - Same firmware for 6V, 9V, or 12V configs
// same as loneoceans lume-x1-avr32dd20 but with Hank-specific defaults

#define HWDEF_H hank/lume-x1/hwdef.h
#include "hank/anduril.h"



//***************************************
//**    RAMP TABLE AND OPERATIONS      **
//***************************************

#define RAMP_SIZE 150   // Change to 160 as well as table and values below if desired

// PWM1: DAC Data
// UDR x^4 curves specifically for Lume-X1 (AVR32DD20)

// levels 1 to 5:
//   ./bin/level_calc.py 5.2 1 5 7135 100 0.01 0.1 --pwm 2500
//   (with the last 2 values divided to fit)
// levels 6 to 150:
//   ./bin/dac-scale.py $( ./bin/level_calc.py 5.3 1 145 7135 17 0.2 3000 --pwm 240000 | grep PWM1 | cut -d : -f 2- )
// top level for each "gear": 3 5 / 39 50 / 123 150
#define PWM1_LEVELS \
         100, 359, 790, 588,1023, \
          17,  21,  24,  29,  34,  39,  46,  54,  62,  71,  82,  94, 107, 122, 138, 157, 177, 199, 223, 250, 279, 311, 346, 385, 426, 471, 520, 573, 630, 692, 758, 830, 907, 990, \
         441, 480, 522, 567, 615, 666, 720, 778, 840, 905, 974, \
          26,  28,  30,  33,  35,  37,  40,  43,  46,  49,  52,  56,  59,  63,  67,  71,  76,  80,  85,  90,  96, 101, 107, 113, 120, 126, 133, 141, 148, 156, 165, 173, 182, 192, 202, 212, 222, 234, 245, 257, 269, 282, 296, 310, 324, 339, 355, 371, 387, 405, 422, 441, 460, 480, 501, 522, 544, 567, 590, 614, 640, 665, 692, 720, 748, 778, 808, 839, 872, 905, 939, 975,1011, \
         429, 445, 461, 478, 495, 513, 531, 550, 570, 589, 610, 631, 653, 675, 698, 721, 745, 770, 795, 821, 848, 875, 903, 932, 962, 992,1023
#define PWM2_LEVELS \
         V10, V10, V10, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25

#define LED_PATH1_PIN_LEVEL_MIN   1
#define LED_PATH2_PIN_LEVEL_MIN   6
#define LED_PATH3_PIN_LEVEL_MIN  51

#define HALFSPEED_LEVEL     20
#define QUARTERSPEED_LEVEL  10

#define DEFAULT_LEVEL  50
#define MAX_1x7135     49

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL   130

#define RAMP_DISCRETE_FLOOR   10
#define RAMP_DISCRETE_CEIL   130
#define RAMP_DISCRETE_STEPS    7

#define SIMPLE_UI_FLOOR   10
#define SIMPLE_UI_CEIL   110      // about ~12W
#define SIMPLE_UI_STEPS    5

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

//***************************************
//**       THERMAL SETTINGS            **
//***************************************

// stop panicking at 6W (not sure of this numbers yet since it depends on the host..)
#define THERM_FASTER_LEVEL 100  // about 6W
#define MIN_THERM_STEPDOWN 60   // similar to single amc7135 in a 3V light

//***************************************
//**       AUX LEDs and MISC           **
//***************************************

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

#define PARTY_STROBE_ONTIME 1  // slow down party strobe
#define STROBE_OFF_LEVEL 1  // keep the regulator chips on between pulses

// smoother candle mode with bigger oscillations
#define CANDLE_AMPLITUDE 30  // default 25

// avr32dd20 has enough space to smooth out voltage readings
#define USE_VOLTAGE_LOWPASS

// enable long-blink as negative sign
#define USE_LONG_BLINK_FOR_NEGATIVE_SIGN

