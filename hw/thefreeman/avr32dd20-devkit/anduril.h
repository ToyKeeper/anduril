// thefreeman's avr32dd20 devkit board
// Copyright (C) 2023 thefreeman, Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define HWDEF_H thefreeman/avr32dd20-devkit/hwdef.h

// HPRsense : 1.7+0.3+5 = 7mR (DMN22M5UFG+trace resistance+5mR)
// Vsense=42.46mV, R1= 191k
// LPRsense : 1R
// transition DAC level 8, ramp level 45
// fifth power ramp 0.1mA to 6066mA

#define RAMP_SIZE 150

// 4 ramp segments:
//   - low  1.024V
//   - low  2.5  V
//   - high 1.024V
//   - high 2.5  V
// HDR ratio: 160
// PWM1: DAC Data
// level_calc.py 4.3287 1 150 7135 5 0.01 1400 --pwm 400000
// top level for each "gear": 30 40 120 150
#define PWM1_LEVELS \
           6,   7,   9,  11,  14,  18,  23,  30,  37,  46,  56,  69,  83, 100, 120, 142, 168, 196, 229, 266, 307, 353, 403, 460, 522, 591, 667, 750, 840, 939, \
         428, 476, 528, 584, 645, 710, 780, 856, 937,1023, \
          17,  18,  20,  21,  23,  25,  27,  29,  32,  34,  37,  40,  42,  46,  49,  52,  56,  60,  64,  68,  72,  77,  81,  86,  92,  97, 103, 109, 115, 122, 128, 136, 143, 151, 159, 167, 176, 185, 194, 204, 214, 224, 235, 246, 258, 270, 283, 295, 309, 323, 337, 352, 367, 383, 399, 416, 434, 452, 470, 489, 509, 529, 550, 572, 594, 617, 640, 664, 689, 715, 741, 768, 796, 824, 854, 884, 915, 947, 979,1013, \
         429, 443, 458, 473, 489, 505, 521, 538, 555, 572, 590, 609, 628, 647, 667, 687, 708, 729, 751, 773, 796, 819, 843, 867, 891, 917, 943, 969, 996,1023
#define PWM2_LEVELS \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25
#define MAX_1x7135            40
#define HDR_ENABLE_LEVEL_MIN  41
#define DEFAULT_LEVEL         50

// no PWM, so MCU clock speed can be slow
#define HALFSPEED_LEVEL      41
#define QUARTERSPEED_LEVEL   40  // seems to run fine at 10kHz/4, try reducing more

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     130  // 50% / 3A / 1000 lm
// 1 22 [44] 65 87 108 130
#define RAMP_DISCRETE_FLOOR  1
#define RAMP_DISCRETE_CEIL   130
#define RAMP_DISCRETE_STEPS  7

// 20 [45] 70 95 120
#define SIMPLE_UI_FLOOR      20
#define SIMPLE_UI_CEIL       120  // ~2.25A / ~750 lm
#define SIMPLE_UI_STEPS      5

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// thermal config

// temperature limit
#define THERM_FASTER_LEVEL  130 // stop panicking at 50%/3A
#define MIN_THERM_STEPDOWN  MAX_1x7135


// UI

#define SIMPLE_UI_ACTIVE 0 // advanced UI by default, because it's a dev board

// allow Aux Config and Strobe Modes in Simple UI
//#define USE_EXTENDED_SIMPLE_UI

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE

#define DEFAULT_2C_STYLE 1 // enable 2 click turbo


// AUX

//#define USE_BUTTON_LED

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// turn on the aux LEDs while main LEDs are on
// because this is a dev board and it's useful to see that
#define USE_AUX_RGB_LEDS_WHILE_ON  20
#define USE_INDICATOR_LED_WHILE_RAMPING

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
#define BLIP_LEVEL 1  // same

// smoother candle mode with bigger oscillations
#define CANDLE_AMPLITUDE 40

// enable 13H factory reset so it can be used on tail e-switch lights
#define USE_SOFT_FACTORY_RESET

// TODO: disable lowpass while asleep; the MCU oversamples

