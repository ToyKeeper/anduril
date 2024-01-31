// Copyright (C) 2017-2023 Selene ToyKeeper
//               2021-2023 loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

//*********************************************
//**  CONFIGURATION OPTIONS FOR LUME-X1-40W  **
//*********************************************

#define HWDEF_H  loneoceans/lume-x1-40w/hwdef.h

//***************************************
//**    RAMP TABLE AND OPERATIONS      **
//***************************************

#define RAMP_SIZE 150

// PWM1: DAC Data
// UDR x^4 curves specifically for Lume X1
#define PWM1_LEVELS 1, 2, 7, 18, 37, 25, 117, 187, 143, 92, 217, 179, 241, 11, 15, 4, 23, 29, 13, 43, 52, 62, 27, 19, 101, 118, 50, 157, 66, 205, 233, 58, 148, 122, 82, 208, 231, 113, 125, 230, 253, 167, 183, 200, 219, 238, 255, 13, 5, 15, 16, 17, 19, 20, 8, 23, 25, 27, 29, 31, 12, 35, 37, 39, 42, 44, 47, 11, 53, 56, 13, 14, 15, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 27, 28, 29, 30, 32, 33, 35, 36, 38, 39, 41, 43, 45, 47, 48, 50, 52, 55, 57, 59, 61, 64, 66, 68, 71, 74, 76, 79, 82, 85, 88, 91, 94, 97, 101, 104, 107, 111, 115, 118, 122, 126, 130, 134, 138, 143, 147, 152, 156, 161, 166, 171, 176, 181, 186, 192, 197, 203, 208, 214, 220, 226, 233, 239, 245, 255

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
#define PWM2_LEVELS V05, V05, V05, V05, V05, V15, V05, V05, V11, V25, V15, V25, V25, V05, V05, V25, V05, V05, V15, V05, V05, V05, V15, V25, V05, V05, V15, V05, V15, V05, V05, V25, V11, V15, V25, V11, V11, V25, V25, V15, V15, V25, V25, V25, V25, V25, V25, V05, V15, V05, V05, V05, V05, V05, V15, V05, V05, V05, V05, V05, V15, V05, V05, V05, V05, V05, V05, V25, V05, V05, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25

// Enable usage of multiple power paths 
//#define USE_MULTIPLE_POWER_PATH (no longer reqired, defined in hwdef.c for 2023 Anduril 2)
#define LED_PATH1_PIN_LEVEL_MIN 1
#define LED_PATH2_PIN_LEVEL_MIN 14
#define LED_PATH3_PIN_LEVEL_MIN 48
#define LED_FET_PIN_LEVEL_MIN 150   // Not used for Lume X1

#define HALFSPEED_LEVEL 52
#define QUARTERSPEED_LEVEL 47   // doublecheck with TK about the mechanics of this

#define DEFAULT_LEVEL 52    // About ~0.5W, ~75 lumens
#define MAX_1x7135 70       // About ~1.65W, ~240/340 lumens M2/P4 
// MIN_THERM_STEPDOWN defined as MAX_1x7135 in ramp-mode.h if not otherwise defined

#define RAMP_SMOOTH_FLOOR 8     // ~0.04 lumens M2? Math says 7 but visually looks like 9
#define RAMP_SMOOTH_CEIL 150    // 5.75A - should be ~3500 lumens M2 or 5klm P4

#define RAMP_DISCRETE_FLOOR RAMP_SMOOTH_FLOOR
#define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 6

#define SIMPLE_UI_FLOOR 16      // should be 10, but visually looks ~16? ~0.13 lumens M2?
#define SIMPLE_UI_CEIL 125      // Should be ~3A, ~2050 lumens M2 or 3klm P4
#define SIMPLE_UI_STEPS 5

// Turn on DC/DC converter enable pin
// For Lume1 - turn on from level 1-149 but not 150
// For Lume X1 - turn on from all levels 1-150
//#define LED_ENABLE_PIN_LEVEL_MIN 1
//#define LED_ENABLE_PIN_LEVEL_MAX 150

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

// stop panicking at ~1.5A (not sure of this numbers yet since it depends on the host..)
#define THERM_FASTER_LEVEL 106          // About ~1.5A/8-9W
#define MIN_THERM_STEPDOWN 70           // About 310mA / 1.7W
#define THERM_NEXT_WARNING_THRESHOLD 16 // 24 by default -> increase for fewer adjustments (more stable output on large time scale)
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

#define DEFAULT_2C_STYLE 1 // enable 2 click turbo

// set this light to use stepped ramp by default (instead of smooth)
#undef RAMP_STYLE
#define RAMP_STYLE 1

// uncomment to disable smooth steps
//#ifdef USE_SMOOTH_STEPS
//#undef USE_SMOOTH_STEPS
//#endif

#define BLINK_BRIGHTNESS (MAX_LEVEL/4)  // increase blink brightness from max/6
#define BLINK_ONCE_TIME 32              // increase from 10 to make brighter

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
#define RGB_LED_OFF_DEFAULT 0x18        // low, rainbow
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
#define PARTY_STROBE_ONTIME 4
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