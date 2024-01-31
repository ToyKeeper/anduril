// Copyright (C) 2017-2023 Selene ToyKeeper
//               2021-2023 Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

//*********************************************
//**   CONFIGURATION OPTIONS FOR LUME1-6AF   **
//*********************************************

#define HWDEF_H  loneoceans/lume1-6af/hwdef.h

//***************************************
//**    RAMP TABLE AND OPERATIONS      **
//***************************************

#define RAMP_SIZE 150

// PWM1: DAC Data
// UDR x^4 curves specifically for Lume1-6AF (with FET for turbo only)
#define PWM1_LEVELS 1, 2, 7, 17, 35, 65, 111, 39, 135, 198, 206, 170, 3, 4, 14, 18, 5, 28, 34, 41, 11, 22, 26, 83, 97, 113, 131, 151, 38, 197, 82, 253, 142, 160, 179, 88, 163, 181, 200, 221, 146, 160, 176, 192, 210, 229, 249, 13, 14, 15, 17, 18, 7, 21, 5, 24, 26, 28, 30, 7, 34, 36, 14, 15, 16, 46, 49, 19, 55, 58, 62, 65, 69, 16, 28, 80, 31, 89, 94, 36, 23, 24, 25, 26, 28, 29, 30, 32, 33, 35, 36, 38, 39, 41, 43, 45, 46, 48, 50, 52, 55, 57, 59, 61, 64, 66, 69, 71, 74, 76, 79, 82, 85, 88, 91, 94, 98, 101, 104, 108, 112, 115, 119, 123, 127, 131, 135, 139, 144, 148, 153, 158, 162, 167, 172, 177, 183, 188, 194, 199, 205, 211, 217, 223, 229, 235, 242, 248, 255, 0

// PWM2_LEVELS - used to set the internal voltage reference for the DAC
//  Makes use the of concept of dynamic Vref for higher resolution output
//  despite DAC being only 8 bits. 
// See Page 165 of datasheet: https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/ATtiny1614-16-17-DataSheet-DS40002204A.pdf
// Bits 2:0 of register VREF.CTRLA are for DAC0 reference:
//  0x0 - 0.55V - ..000
//  0x1 - 1.1V -  ..001
//  0x2 - 2.5V -  ..010
//  0x3 - 4.3V -  ..011
//  0x4 - 1.5V -  ..100
// Bits 6:4 of register VREF.CTRLA are for ADC0 reference:
//  We can set this to be 0x2 for 2.5V reference
// Hence set PWM2_LEVELS to be 10000 (0d16) for 0.55V DAC vref (V05/V055)
//       set PWM2_LEVELS to be 10001 (0d17) for 1.1V DAC vref (V11)
//       set PWM2_LEVELS to be 10100 (0d20) for 1.5V DAC vref (V15)
//       set PWM2_LEVELS to be 10010 (0d18) for 2.5V DAC vref (V25)
// VREF selector (0.55V=16,1.1V=17, 2.5V=18, 4.3V=19, 1.5V=20)
#define PWM2_LEVELS V05, V05, V05, V05, V05, V05, V05, V25, V11, V11, V15, V25, V15, V15, V05, V05, V25, V05, V05, V05, V25, V15, V15, V05, V05, V05, V05, V05, V25, V05, V15, V05, V11, V11, V11, V25, V15, V15, V15, V15, V25, V25, V25, V25, V25, V25, V25, V05, V05, V05, V05, V05, V15, V05, V25, V05, V05, V05, V05, V25, V05, V05, V15, V15, V15, V05, V05, V15, V05, V05, V05, V05, V05, V25, V15, V05, V15, V05, V05, V15, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25

// PWM3_LEVELS - 8-bit PWM values for FET
/*
    Generally used for Turbo only by default, this driver uses 
    the regulated buck for levels 1-149, and 100% FET for turbo
    at level 150. Customize it to your liking. 
    Do not turn on FET and Buck at the same time; ensure ramp tables line up. 
    It is possible to run FET at 8-bit PWM if desired. Operational frequency
    is the standard ~20kHz. 
*/
#define PWM3_LEVELS 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255

// Enable usage of multiple power paths 
//#define USE_MULTIPLE_POWER_PATH (no longer reqired, defined in hwdef.c for 2023 Anduril)
#define LED_PATH1_PIN_LEVEL_MIN 1
#define LED_PATH2_PIN_LEVEL_MIN 13
#define LED_PATH3_PIN_LEVEL_MIN 48
#define LED_FET_PIN_LEVEL_MIN 150

// turn on Buck from level 1 to 149, but not 150
// TODO: already handled by reading PWM tables but requires PWM levels to have 0s appropriately
//#define BCK_ENABLE_PIN_LEVEL_MIN 1
//#define BCK_ENABLE_PIN_LEVEL_MAX 149

// Turn on DC/DC converter enable pin
// For Lume1 - turn on from level 1-149 but not 150
// For Lume X1 - turn on from all levels 1-150
//#define LED_ENABLE_PIN_LEVEL_MIN 1
//#define LED_ENABLE_PIN_LEVEL_MAX 150

#define HALFSPEED_LEVEL 47
#define QUARTERSPEED_LEVEL 39   // check with TK about the mechanics of this

#define DEFAULT_LEVEL 61        // About ~0.5W, ~70 lumens
#define MAX_1x7135 88           // About ~2.25W, ~340 lumens
// MIN_THERM_STEPDOWN defined as MAX_1x7135 in ramp-mode.h if not otherwise defined

#define RAMP_SMOOTH_FLOOR 8     // ~0.04 lumens M2? Math says 7 but visually looks like 9
#define RAMP_SMOOTH_CEIL 149    // Level 150 is when Buck is off and FET is ON 100%

#define RAMP_DISCRETE_FLOOR RAMP_SMOOTH_FLOOR
#define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 6

#define SIMPLE_UI_FLOOR 13
#define SIMPLE_UI_CEIL 142      // Should be ~5A
#define SIMPLE_UI_STEPS 5

// don't blink mid-ramp
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

//***************************************
//**       THERMAL SETTINGS            **
//***************************************

// set thermal ceiling
#ifdef DEFAULT_THERM_CEIL
#undef DEFAULT_THERM_CEIL
#endif
#define DEFAULT_THERM_CEIL 50

// extra boost during turbo, lower when temp = temp_ceil + extra
#define TURBO_TEMP_EXTRA 10

// stop panicking at ~1.5A (not sure of this numbers yet since it depends on the host..)
#define THERM_FASTER_LEVEL 125          // About 8-9W
#define MIN_THERM_STEPDOWN 88           // About 2.2W
#define THERM_NEXT_WARNING_THRESHOLD 28 // 24 by default -> increase for fewer adjustments (more stable output on large time scale)
#define THERM_RESPONSE_MAGNITUDE 32     // 64 by default -> decrease for smaller adjustments (removes dip post turbo)
//#define THERM_WINDOW_SIZE 1           // 2 by default -> decrease for tighter temperature regulation
//#define THERM_LOOKAHEAD 4             // 4 by default -> decrease for longer turbo

// easier access to thermal config mode, similar to Emisar, Noctigon
//#define USE_TENCLICK_THERMAL_CONFIG

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

#define USE_BUTTON_LED

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// show each channel while it scroll by in the menu
#define USE_CONFIG_COLORS

// blink numbers on the main LEDs by default
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
#define PARTY_STROBE_ONTIME 2
#define STROBE_OFF_LEVEL 1  // keep regulators on between strobes

// smoother candle mode with bigger oscillations
#define CANDLE_AMPLITUDE 30 // default 25

// attiny1616 has enough space to smooth out voltage readings
#define USE_VOLTAGE_LOWPASS

// can reset the normal way because power is not connected before the button, but do this anyway! 
#define USE_SOFT_FACTORY_RESET

// enable long-blink as negative sign
#define USE_LONG_BLINK_FOR_NEGATIVE_SIGN

// Uncomment to use Microphone mode
//#define USE_MICROPHONE_MODE
//#define MICROPHONE_SCALER 10

// ======================================