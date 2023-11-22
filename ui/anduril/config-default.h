// config-default.h: Default configuration for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

/*
 * This file specifies the default settings for Anduril.
 *
 * These settings can be overridden per build target, in cfg-*.h files...
 * ... but most are not.  So changing one here will make it change in
 * almost every build target.
 *
 * Some configurable settings are also in other *.h files.
 */

/********* User-configurable options *********/
// low voltage protection (also required for battery check mode)
#define USE_LVP

// overheat protection
#define USE_THERMAL_REGULATION
#if (MCU==0x85) || (MCU==0x1634)
// sloppy temperature sensor needs bigger error margin
#define DEFAULT_THERM_CEIL 45  // try not to get hotter than this (in C)
#else
// more accurate temperature sensor can regulate higher safely
#define DEFAULT_THERM_CEIL 50  // try not to get hotter than this (in C)
#endif
// Comment out to disable automatic calibration on factory reset
//   - If so, be sure to set THERM_CAL_OFFSET to the correct calibration offset
//   - Calibration can still be overridden in temperature check mode
// Or uncomment to use the default auto-calibrate on factory reset
//
// To determine THERM_CAL_OFFSET, comment out USE_THERM_AUTOCALIBRATE to
// disable auto-calibration, compile and flash, let flashlight rest at a known
// temperature, then enter temp check mode (do NOT enter calibration mode).
//
// THERM_CAL_OFFSET = known_temperature - temp_check_blinks + THERM_CAL_OFFSET
//
// (include THERM_CAL_OFFSET in sum as it might already be a non-zero number)
#define USE_THERM_AUTOCALIBRATE

// Include a simplified UI for non-enthusiasts?
#define USE_SIMPLE_UI


///// Ramp mode options /////

// button timing for turning light on/off:
// B_PRESS_T:   activate as soon as button is pressed
// B_RELEASE_T: activate when user lets go of button
// B_TIMEOUT_T: activate when we're sure the user won't double-click
// defaults are release on, timeout off
#define B_TIMING_ON B_RELEASE_T
#define B_TIMING_OFF B_TIMEOUT_T

// default ramp style: 0 = smooth, 1 = stepped
#define RAMP_STYLE 0

// smooth ramp speed: 1, 2, 3, 4, ...  for 1X speed, 1/2, 1/3rd, 1/4th, ...
#define USE_RAMP_SPEED_CONFIG

// after ramping, how long until the direction resets to "up"?
#define AUTO_REVERSE_TIME (TICKS_PER_SECOND * 2 / 3)

// add runtime option for whether hold-from-off should ramp or stay at moon
#define USE_RAMP_AFTER_MOON_CONFIG

// short blip when crossing from "click" to "hold" from off
// (helps the user hit moon mode exactly, instead of holding too long
//  or too short)
#define MOON_TIMING_HINT  // only applies if B_TIMING_ON == B_PRESS_T
// short blips while ramping
#define BLINK_AT_RAMP_MIDDLE
//#define BLINK_AT_RAMP_FLOOR
#define BLINK_AT_RAMP_CEIL
//#define BLINK_AT_STEPS  // whenever a discrete ramp mode is passed in smooth mode

// Uncomment for Anduril1 "Ramp 2C" behavior:
//   - Ramp 2C goes to turbo (advanced UI) or ceiling (simple UI), like in Anduril1
// Or comment out to use Anduril2 behavior instead:
//   - Ramp 2C goes to ceiling, unless already at ceiling or in simple UI.
//     (Advanced UI ceiling 2C goes to turbo)
//#define USE_2C_MAX_TURBO
// Or uncomment to let the user decide which style they want:
#define USE_2C_STYLE_CONFIG
// 0 = no turbo
// 1 = A1 style: Off 2C = ceil, On 2C = turbo
// 2 = A2 style: Off 2C = ceil, On 2C = ceil, Ramped ceil 2C = turbo
// All styles allow momentary turbo in advanced UI
//#define DEFAULT_2C_STYLE 2  // default to Anduril 2 style
//#define DEFAULT_2C_STYLE_SIMPLE 0  // no turbo at all

// make the ramps configurable by the user
#define USE_RAMP_CONFIG

// adds a runtime option to switch between automatic memory (default)
// and manual memory (only available if compiled in)
// (manual memory makes 1-click-from-off start at the same level each time)
// (the level can be set explicitly with 10 clicks from on,
// or the user can go back to automatic with 10H)
#define USE_MANUAL_MEMORY
// if enabled, user can use "hybrid memory"
// The light will use automatic or manual memory level, depending on how long
// the light was off.  Short off = automatic, long off = manual.
// This also remaps 10C/10H:
// - 10C toggles manual mem on/off at current level.
// - 10H configures the timer value.
#define USE_MANUAL_MEMORY_TIMER

// enable sunset timer (ramp-down and automatic shutoff)
// timer is available in regular ramp mode and candle mode
#define USE_SUNSET_TIMER

// optionally make gradual ticks happen faster
// Affects: thermal regulation speed, sunset mode, maybe other features
// (default is calibrated for 8-bit PWM,
//  but 10-bit should set this value to 4 instead of 1)
#define GRADUAL_ADJUST_SPEED  1


///// What to do when power is connected /////
// factory reset function erases user's runtime configuration in eeprom
#define USE_FACTORY_RESET
//#define USE_SOFT_FACTORY_RESET  // only needed on models which can't use hold-button-at-boot

// dual-switch support (second switch is a tail clicky)
// (currently incompatible with factory reset)
//#define START_AT_MEMORIZED_LEVEL


///// extra modes (enable / disable / configure each mode) /////

// include a function to blink out the firmware version
#define USE_VERSION_CHECK

// enable the battery check mode (shows remaining charge) (requires USE_LVP)
#define USE_BATTCHECK_MODE
// battery readout style (pick one)
// TODO: allow VpT and 4-bar simultaneously,
//       so one can be in "simple mode" and the other in "advanced mode"
#define BATTCHECK_VpT
//#define BATTCHECK_8bars  // FIXME: breaks build
//#define BATTCHECK_4bars  // FIXME: breaks build
#if ROM_SIZE > 10000
    // battcheck displays 1.25V instead of 1.2V
    #define USE_EXTRA_BATTCHECK_DIGIT
#endif
// allow the user to calibrate the voltage readings?
// (adjust in 0.05V increments from -0.30V to +0.30V)
// (1 = -0.30V, 2 = -0.25V, ... 7 = 0V, ... 13 = +0.30V)
#define USE_VOLTAGE_CORRECTION

// enable beacon mode
#define USE_BEACON_MODE

// enable/disable various strobe modes
#define USE_BIKE_FLASHER_MODE
#define USE_PARTY_STROBE_MODE
#define USE_TACTICAL_STROBE_MODE
#define USE_LIGHTNING_MODE
#define USE_CANDLE_MODE

// boring strobes nobody really likes, but sometimes flashlight companies want
// (these replace the fun strobe group,
//  so don't enable them at the same time as any of the above strobes)
//#define USE_POLICE_STROBE_MODE
#define USE_SOS_MODE
//#define USE_SOS_MODE_IN_FF_GROUP  // put SOS in the "boring strobes" mode
#define USE_SOS_MODE_IN_BLINKY_GROUP  // put SOS in the blinkies mode group

// enable a mode for locking the light for safe carry
#define USE_LOCKOUT_MODE
// should lockout mode function as a momentary moon mode?
#define USE_MOON_DURING_LOCKOUT_MODE
// add an optional setting to lock the light after being off for a while
#define USE_AUTOLOCK

// enable momentary mode
#define USE_MOMENTARY_MODE

// enable tactical mode
#define USE_TACTICAL_MODE


// enable a shortcut for +10 in number entry mode
// (click for +1, hold for +10)
#define USE_NUMBER_ENTRY_PLUS10

// cut clock speed at very low modes for better efficiency
// (defined here so config files can override it)
#define USE_DYNAMIC_UNDERCLOCKING

// if the aux LEDs oscillate between "full battery" and "empty battery"
// while in "voltage" mode, enable this to reduce the amplitude of
// those oscillations
#if (ROM_SIZE > 10000)
#define USE_LOWPASS_WHILE_ASLEEP
#endif

// if there's tint ramping, allow user to set it smooth or stepped
#define USE_STEPPED_TINT_RAMPING
#define DEFAULT_TINT_RAMP_STYLE 0  // smooth

// Use "smooth steps" to soften on/off and step changes
// on MCUs with enough room for extra stuff like this
#if (ROM_SIZE > 10000)
#define USE_SMOOTH_STEPS
#endif
// 0 = none, 1 = smooth, 2+ = undefined
#define DEFAULT_SMOOTH_STEPS_STYLE  1

// by default, allow user to set the channel for each strobe-group mode
// (but allow disabling this feature per build)
#define USE_CHANNEL_PER_STROBE

