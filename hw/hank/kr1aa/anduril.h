// Emisar / Noctigon KR1AA config options for Anduril
// Copyright (C) 2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define HWDEF_H hank/emisar-d3aa/hwdef.h
#include "hank/anduril.h"
#include "hank/emisar-d3aa/anduril.h"

// wait this many ms at boot before testing the battery
// (or doing anything else)
// (to give the user more time to make a solid electrical connection)
#define WEAK_BATTERY_TEST_DELAY  500

#undef RAMP_SIZE
#define RAMP_SIZE 150

// 4 ramp segments:
//   - low  1.024V
//   - low  2.5  V
//   - high 1.024V
//   - high 2.5  V
// 2nd gear starts at 23:
// ./bin/dac-scale.py $( ./bin/level_calc.py 3.906 1 150 7135 3 0.01 1400 --pwm 285400 | grep PWM1 | cut -d : -f 2- )
// top level for each "gear": 30 40 118 150
// 2nd gear starts at 24:
// ./bin/dac-scale.py $( ./bin/level_calc.py 3.856 1 150 7135 3 0.01 1400 --pwm 273200 | grep PWM1 | cut -d : -f 2- )
// top level for each "gear": 30 40 117 150
#undef PWM1_LEVELS
#define PWM1_LEVELS \
           3,   5,   6,   9,  12,  16,  21,  28,  36,  46,  57,  71,  86, 105, 126, 150, 177, 208, 243, 282, 326, 374, 428, 487, 552, 623, 700, 785, 877, 977, \
         444, 491, 543, 598, 658, 722, 790, 862, 940,1023, \
          24,  26,  29,  31,  33,  36,  39,  42,  45,  48,  52,  55,  59,  63,  67,  71,  76,  81,  86,  91,  96, 102, 108, 114, 120, 127, 134, 141, 149, 157, 165, 173, 182, 191, 200, 210, 220, 230, 241, 252, 264, 276, 288, 301, 314, 327, 341, 355, 370, 385, 401, 417, 434, 451, 469, 487, 505, 525, 544, 564, 585, 607, 628, 651, 674, 698, 722, 747, 773, 799, 826, 853, 881, 910, 940, 970,1001, \
         423, 436, 450, 463, 478, 492, 507, 522, 538, 554, 570, 586, 603, 621, 638, 656, 675, 693, 712, 732, 752, 772, 793, 814, 836, 858, 880, 903, 926, 950, 974, 998,1023
#undef PWM2_LEVELS
#define PWM2_LEVELS \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, \
         V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, V10, \
         V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25
#define MAX_1x7135            40
#undef MAX_1x7135
#define HDR_ENABLE_LEVEL_MIN  41
#define MAX_1x7135            40

#undef DEFAULT_LEVEL
#define DEFAULT_LEVEL         50

// no PWM, so MCU clock speed can be slow
#undef HALFSPEED_LEVEL
#define HALFSPEED_LEVEL      41
#undef QUARTERSPEED_LEVEL
#define QUARTERSPEED_LEVEL   40  // seems to run fine at 10kHz/4, try reducing more?


// AUX

// there is no lighted button
// (but it doesn't hurt to leave this enabled,
//  and then this firmware can be used on D3AA too)
//#ifdef USE_AUX1_LED
//#undef USE_AUX1_LED
//#endif


