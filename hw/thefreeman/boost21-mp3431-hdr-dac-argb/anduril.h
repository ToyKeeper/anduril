// thefreeman's BST21 BST20-FWxA (no button LED)
// Copyright (C) 2023 TBD (thefreeman), Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define HWDEF_H  thefreeman/boost21-mp3431-hdr-dac-argb/hwdef.h

// HPRsense : 1.7+0.3+5 = 7mR (DMN22M5UFG+trace resistance+5mR)
// Vsense=42.46mV, R1= 191k
// LPRsense : 1R
// transition DAC level 8, ramp level 45
// fifth power ramp 0.1mA to 6066mA

#define RAMP_SIZE 150

// 4 ramp segments:
//   - low 0.55V
//   - low 2.5V
//   - high 0.55V
//   - high 2.5V
// PWM1: DAC Data
#define PWM1_LEVELS \
           2,  3,  4,  5,  7,  9, 11, 13, 16, 19, 23, 28, 33, 39, 45, 53, 61, 71, 81, 93,106,121,137,155,175,196,220,246, \
          60, 67, 74, 82, 91,100,110,121,133,146,159,174,190,207,224,244, \
          8,  9,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20, 21, 23, 24, 26, 27, 29, 31, 33, 35, 37, 40, 42, 45, 47, 50, 53, 56, 59, 62, 66, 69, 73, 77, 81, 85, 90, 94, 99,104,109,114,120,126,132,138,144,151,158,165,173,180,188,196,205,214,223,232,242,252, \
         57, 60, 62, 65, 67, 70, 73, 76, 78, 82, 85, 88, 91, 95, 98,102,105,109,113,117,121,126,130,135,139,144,149,154,159,164,170,175,181,187,193,199,206,212,219,225,232,240,247,255
// PWM2: VREF selector (0.55V=16,1.1V=17, 2.5V=18, 4.3V=19, 1.5V=20)
#define PWM2_LEVELS \
        V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05, \
        V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25, \
        V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05,V05, \
        V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25,V25

#define MAX_1x7135           44
#define DEFAULT_LEVEL        44
#define HDR_ENABLE_LEVEL_MIN 45 // when HDR FET turns ON

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

//#define THERM_LOOKAHEAD 4 // 4 by default -> decrease for longer turbo
#define THERM_NEXT_WARNING_THRESHOLD 48 // 24 by default -> increase for fewer adjustments (more stable output on large time scale)
#define THERM_RESPONSE_MAGNITUDE 32 // 64 by default -> decrease for smaller adjustments (removes dip post turbo)
//#define THERM_WINDOW_SIZE 1 // 2 by default -> decrease for tighter temperature regulation


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

