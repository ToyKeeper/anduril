// Fireflies Lume-X1 config options for Anduril
// Copyright (C) 2018-2026 Selene ToyKeeper, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as parent model but different power limit

#include "fireflies/lume-x1/avr32dd20/anduril.h"

#undef PWM1_LEVELS
#undef PWM2_LEVELS
// maximum 70% power for 32W turbo  (12V 2.5A / 6V 5A ?)
// levels 1 to 5:
//   ./bin/level_calc.py 5.2 1 5 7135 100 0.01 0.1 --pwm 2500
//   (with the last 2 values divided to fit)
// levels 6 to 150:
//   ./bin/dac-scale.py 716 1023 $( ./bin/level_calc.py 5.069 1 145 7135 24 0.2 2700 --pwm 320000 | grep PWM1 | cut -d : -f 2- )
// top level for each "gear": 3 5 / 38 50 / 132 150
#define PWM1_LEVELS \
         100, 359, 790, 588,1023, \
          16,  20,  24,  29,  34,  40,  47,  55,  65,  75,  86,  99, 114, 129, 147, 167, 188, 212, 238, 267, 298, 333, 370, 410, 454, 503, 555, 611, 671, 736, 807, 883, 964, \
         430, 468, 509, 553, 600, 650, 702, 759, 819, 883, 951,1023, \
          20,  22,  24,  25,  27,  29,  31,  33,  36,  38,  40,  43,  46,  49,  52,  55,  58,  62,  65,  69,  73,  77,  82,  86,  91,  96, 101, 107, 113, 118, 125, 131, 138, 145, 152, 159, 167, 175, 184, 192, 201, 211, 220, 230, 241, 252, 263, 275, 287, 299, 312, 325, 339, 353, 368, 383, 399, 415, 432, 449, 467, 485, 504, 523, 544, 564, 586, 608, 630, 654, 678, 702, 728, 754, 781, 809, 837, 866, 897, 927, 959, 992, \
         420, 434, 448, 463, 478, 494, 510, 527, 544, 561, 579, 597, 616, 635, 654, 674, 695, 716
#define PWM2_LEVELS \
         V10, V10, V10, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25

#undef LED_PATH1_PIN_LEVEL_MIN
#undef LED_PATH2_PIN_LEVEL_MIN
#undef LED_PATH3_PIN_LEVEL_MIN
#define LED_PATH1_PIN_LEVEL_MIN   1
#define LED_PATH2_PIN_LEVEL_MIN   6
#define LED_PATH3_PIN_LEVEL_MIN  51


// lower temperature limit
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 45

