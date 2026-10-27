// Fireflies Lume1-6af-nofet config options for Anduril
// Copyright (C) 2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// Same as the lume1-6af, but without the DD FET turbo at level 150
#define NO_DDFET_TURBO
#include "fireflies/lume1/6af/anduril.h"


#undef PWM1_LEVELS
#undef PWM2_LEVELS
// levels 1 to 5:
//   ./bin/level_calc.py 5.2 1 5 7135 150 0.01 0.1 --pwm 2500
//   150,403,826,1493,2500
//   (scaled by `x*255/550` or `x*255/2500` to fit)
// levels 6 to 150:
//   ./bin/dac-scale.py 255 255 $( ./bin/level_calc.py 5.88 1 145 7135 26 0.5 3000 --pwm 230000 | grep PWM1 | cut -d : -f 2- )
// top level for each "gear": 2 5 / 30 50 / 107 150
#define PWM1_LEVELS \
         69, 186,  \
         84, 152, 255,  \
         12,  14,  17,  20,  24,  28,  32,  38,  43,  49,  56,  63,  71,  79,  89,  99, 111, 123, 136, 150, 166, 183, 201, 221, 242, \
         58,  63,  69,  75,  82,  89,  96, 105, 113, 123, 132, 143, 154, 166, 179, 192, 206, 221, 237, 254, \
         13,  14,  15,  16,  17,  18,  19,  21,  22,  23,  25,  27,  28,  30,  32,  34,  36,  38,  40,  42,  45,  47,  50,  53,  56,  59,  62,  65,  69,  72,  76,  80,  84,  88,  93,  97, 102, 107, 112, 118, 123, 129, 135, 141, 148, 155, 162, 169, 177, 184, 192, 201, 210, 219, 228, 238, 248, \
         56,  59,  61,  64,  66,  69,  72,  75,  78,  81,  84,  87,  90,  94,  97, 101, 105, 109, 113, 117, 121, 126, 130, 135, 140, 145, 150, 155, 161, 166, 172, 178, 184, 190, 197, 203, 210, 217, 224, 231, 239, 247, 255
#define PWM2_LEVELS \
        V05, V05,  \
        V25, V25, V25,  \
        V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, \
        V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
        V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, V05, \
        V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25


// no weird turbo, so no need to disable this
#define BLINK_AT_RAMP_CEIL

