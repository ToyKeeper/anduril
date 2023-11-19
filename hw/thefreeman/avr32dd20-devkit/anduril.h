// thefreeman's BST21 BST20-FWxA (no button LED)
// Copyright (C) 2023 TBD (thefreeman), Selene ToyKeeper
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
#if 1
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
#else
// level_calc.py 9.21 1 150 7135 5 0.2 1400 --pwm 400000
// (plus dac-scale.py post-processing to get values for HDR+Vref ranges)
// top level for each "gear": 35 48 127 150
#define PWM1_LEVELS \
           5,  11,  18,  25,  33,  41,  50,  60,  71,  83,  96, 110, 125, 141, 158, 177, 198, 220, 244, 269, 297, 326, 358, 392, 429, 469, 511, 556, 605, 657, 713, 772, 836, 904, 976, \
         431, 465, 501, 539, 580, 624, 670, 720, 772, 828, 887, 950,1017, \
          16,  17,  18,  20,  21,  23,  24,  26,  27,  29,  31,  33,  35,  37,  40,  42,  45,  48,  51,  54,  57,  60,  64,  68,  72,  76,  80,  85,  90,  95, 100, 106, 112, 118, 124, 131, 138, 145, 153, 161, 170, 179, 188, 198, 208, 219, 230, 242, 254, 266, 280, 294, 308, 323, 339, 355, 373, 391, 409, 429, 449, 470, 492, 515, 539, 564, 589, 616, 644, 673, 704, 735, 768, 802, 837, 874, 912, 952, 993, \
         424, 442, 461, 480, 501, 522, 544, 566, 590, 614, 640, 666, 693, 721, 750, 780, 811, 844, 877, 912, 948, 985,1023
// Vref selector (V10, V20, V25, V40 = 1.024V, 2.048V, 2.5V, 4.096V)
#define PWM2_LEVELS \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25

#define MAX_1x7135           48
#define DEFAULT_LEVEL        48
#define HDR_ENABLE_LEVEL_MIN 49 // when HDR FET turns ON
#endif

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

//#define SIMPLE_UI_ACTIVE 0 // advanced UI by default

// allow Aux Config and Strobe Modes in Simple UI
//#define USE_EXTENDED_SIMPLE_UI

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE

#define DEFAULT_2C_STYLE 1 // enable 2 click turbo


// AUX

//#define USE_BUTTON_LED

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

// TODO: disable lowpass while asleep; the MCU oversamples

