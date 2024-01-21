// lume1-2020 config options for Anduril
// Copyright (C) 2020-2023 Selene ToyKeeper, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

//*********************************************
//**   CONFIGURATION OPTIONS FOR LUME1-2020  **
//*********************************************

#define HWDEF_H  loneoceans/lume1-2020/hwdef.h

//***************************************
//**    RAMP TABLE AND OPERATIONS      **
//***************************************

// level_calc.py seventh 1 149 7135 16 0.2 2000 --pwm 32640, with 0 appended to the end
// regulated channel: 16-bit PWM+DSM for low lows and very smooth ramp
// for FET PWM (PWM2), all values are 0, except for last value of 255
// .. but user can configure for smooth FET if desired...

// For FET version (length 150)
#define RAMP_SIZE 150
//#define PWM1_LEVELS 1,2,3,3,3,3,3,4,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,12,13,14,15,16,17,18,20,21,23,24,26,27,29,31,33,35,37,39,41,43,45,48,50,53,56,58,61,64,67,70,74,77,80,84,88,91,95,99,103,108,112,116,121,125,130,135,140,145,150,156,161,167,173,178,184,191,197,203,210,217,223,230,238,245,252,260,268,275,283,292,300,308,317,326,335,344,353,363,372,382,392,402,413,423,434,445,456,467,478,490,502,514,526,538,551,563,576,589,603,616,630,644,658,672,687,702,717,732,747,763,778,794,811,827,844,861,878,895,913,931,949,967,985,1004,1023,0
#define PWM1_LEVELS 16,17,17,18,18,19,20,21,22,23,24,25,26,28,30,31,33,35,38,40,43,46,49,52,56,60,64,68,73,79,84,90,97,104,112,120,128,137,147,158,169,181,194,207,222,237,253,270,289,308,329,350,374,398,424,451,480,510,543,576,612,650,690,731,775,822,870,921,975,1032,1091,1153,1218,1287,1358,1434,1512,1595,1681,1771,1866,1965,2068,2176,2289,2406,2529,2657,2791,2931,3077,3228,3387,3551,3723,3902,4088,4281,4483,4693,4911,5137,5373,5617,5872,6136,6410,6695,6990,7297,7615,7945,8287,8642,9009,9390,9785,10194,10618,11056,11510,11980,12466,12969,13490,14028,14584,15160,15754,16369,17004,17660,18338,19038,19760,20507,21277,22071,22892,23738,24610,25511,26439,27396,28383,29399,30448,31527,32640, 0
// DD FET: 8-bit PWM (hardware can handle PWM but preference is on or off)
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255

// level 150 is when Buck is off and FET is ON 100%
// adjust these values if ramp table is changed
// no longer required, buck turns off if FET is on..
//#define LED_ENABLE_PIN_LEVEL_MIN 1
//#define LED_ENABLE_PIN_LEVEL_MAX 149

#define DEFAULT_LEVEL 42  // About ~250mA
#define MAX_1x7135 149

#define HALFSPEED_LEVEL 14
#define QUARTERSPEED_LEVEL 5

// the entire ramp is regulated; don't blink halfway up
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL 149

#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 6

#define SIMPLE_UI_FLOOR 15
#define SIMPLE_UI_CEIL 142
#define SIMPLE_UI_STEPS 5

//***************************************
//**       THERMAL SETTINGS            **
//***************************************

// set thermal ceiling
#ifdef DEFAULT_THERM_CEIL
#undef DEFAULT_THERM_CEIL
#endif
#define DEFAULT_THERM_CEIL 50

// extra boost during turbo, lower when temp = temp_ceil + extra
#define TURBO_TEMP_EXTRA 7

// stop panicking at ~1.5A (not sure of this numbers yet since it depends on the host..)
#define THERM_FASTER_LEVEL 116
#define MIN_THERM_STEPDOWN 94           // about 300 lm
#define THERM_NEXT_WARNING_THRESHOLD 28 // 24 by default -> increase for fewer adjustments (more stable output on large time scale)
#define THERM_RESPONSE_MAGNITUDE 32     // 64 by default -> decrease for smaller adjustments (removes dip post turbo)
//#define THERM_WINDOW_SIZE 1           // 2 by default -> decrease for tighter temperature regulation
//#define THERM_LOOKAHEAD 4             // 4 by default -> decrease for longer turbo

//***************************************
//**          USER INTERFACE           **
//***************************************

//#define SIMPLE_UI_ACTIVE 0 // advanced UI by default

// allow Aux Config and Strobe Modes in Simple UI
//#define USE_EXTENDED_SIMPLE_UI

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE

#define DEFAULT_2C_STYLE 1 // enable 2 click to turbo from on

// set this light to use stepped ramp by default (instead of smooth)
#undef RAMP_STYLE
#define RAMP_STYLE 1

// uncomment to disable smooth steps
//#ifdef USE_SMOOTH_STEPS
//#undef USE_SMOOTH_STEPS
//#endif

#define BLINK_BRIGHTNESS (MAX_LEVEL/4)  // increase blink brightness from max/6
#define BLINK_ONCE_TIME 36              // increase from 10 to make brighter

//***************************************
//**       AUX LEDs and MISC           **
//***************************************

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default (but allow user to change it)
#define DEFAULT_BLINK_CHANNEL  CM_MAIN

// this light only has one main led channel
// use aux red + aux blue for police strobe
#define USE_POLICE_COLOR_STROBE_MODE
#define POLICE_STROBE_USES_AUX
#define POLICE_COLOR_STROBE_CH1 CM_AUXRED
#define POLICE_COLOR_STROBE_CH2 CM_AUXBLU

// the aux LEDs are front-facing, so turn them off while main LEDs are on
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

// RGB aux LEDs should use rainbow cycling mode
// to impress new customers
// (people usually change it to voltage mode later though)
#ifdef RGB_LED_OFF_DEFAULT
#undef RGB_LED_OFF_DEFAULT
#endif
#define RGB_LED_OFF_DEFAULT     0x18    // low, rainbow
#define RGB_LED_LOCKOUT_DEFAULT 0x37    // blinking, rainbow

// enable blinking aux LEDs
#define TICK_DURING_STANDBY
#define STANDBY_TICK_SPEED 3  // every 0.128 s
//#define STANDBY_TICK_SPEED 4  // every 0.256 s
//#define STANDBY_TICK_SPEED 5  // every 0.512 s

// enable SOS mode
#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

// enable Beacontower blinky mode 
#define USE_BEACONTOWER_MODE

// party strobe on-time
#define PARTY_STROBE_ONTIME 3
#define STROBE_OFF_LEVEL 1  // keep regulators on between strobes

// smoother candle mode with bigger oscillations
#define CANDLE_AMPLITUDE 30 // default 25

// attiny1634 has enough flash to allow this feature
#define USE_VOLTAGE_LOWPASS

// can reset the normal way because power is not connected before the button, but do this anyway! 
#define USE_SOFT_FACTORY_RESET

// enable long-blink as negative sign
#define USE_LONG_BLINK_FOR_NEGATIVE_SIGN

// ======================================
