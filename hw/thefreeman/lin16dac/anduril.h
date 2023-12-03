// thefreeman's Linear 16 driver using DAC control
// Copyright (C) 2021-2023 (FIXME)
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define HWDEF_H  thefreeman/lin16dac/hwdef.h

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING
// off mode: low (1)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)

// level_calc.py ninth 2 150 7135 1 0.03 6.4 7135 1 6.3 1600
#define RAMP_SIZE 150

// 4 ramp segments:
//   - low 0.55V
//   - low 2.5V
//   - high 0.55V
//   - high 2.5V
// PWM1: DAC Data
// FIXME: ramp stalls with 8 duplicate levels in a row
//        (maybe use 1.1V Vref during that part of the ramp?)
#define PWM1_LEVELS \
         25, 25, 33, 41, 41, 50, 58, 66, 75, 83, 92,108,117,133,150,167,192,209,234, \
         58, 64, 71, 80, 90, 99,110,121,134,149,163,180,198,218,241, \
          1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9, 10, 11, 11, 12, 13, 14, 15, 16, 18, 19, 20, 22, 23, 25, 26, 28, 30, 32, 34, 36, 39, 41, 44, 47, 50, 53, 56, 59, 63, 67, 71, 75, 79, 84, 89, 94,100,105,112,118,124,131,139,146,154,163,172,181,191,201,212,223,234,246, \
         57, 60, 63, 66, 69, 73, 76, 80, 84, 88, 93, 97,102,107,112,117,123,129,135,141,147,154,161,169,176,184,193,201,210,220,229,239,250,255
// PWM2: VREF selector (0.55V=16,1.1V=17, 2.5V=18, 4.3V=19, 1.5V=20)
#define PWM2_LEVELS \
        V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05, \
        V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25, \
        V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05, \
        V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25

#define MAX_1x7135            34
#define HDR_ENABLE_LEVEL_MIN  35  // bottom level of top half of the ramp
#define HALFSPEED_LEVEL       255  // always run at 1/4th speed
#define QUARTERSPEED_LEVEL    255  // because DAC doesn't use PWM

#define RAMP_SMOOTH_FLOOR    1
#define RAMP_SMOOTH_CEIL     120
// 10, 28, 46, [65], 83, 101, 120
#define RAMP_DISCRETE_FLOOR  10
#define RAMP_DISCRETE_CEIL   RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS  7

// stop panicking at ~30% power
#define THERM_FASTER_LEVEL 123

// enable 2 click turbo
#define DEFAULT_2C_STYLE 1

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

