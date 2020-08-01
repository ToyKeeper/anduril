/*
 * config-default.h: Default configuration for Anduril.
 *
 * Copyright (C) 2017 Selene ToyKeeper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIG_DEFAULT_H
#define CONFIG_DEFAULT_H

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
#define DEFAULT_THERM_CEIL 45  // try not to get hotter than this (in C)

// factory reset function erases user's runtime configuration in eeprom
#define USE_FACTORY_RESET
//#define USE_SOFT_FACTORY_RESET  // only needed on models which can't use hold-button-at-boot

// dual-switch support (second switch is a tail clicky)
// (currently incompatible with factory reset)
//#define START_AT_MEMORIZED_LEVEL

// include a function to blink out the firmware version
#define USE_VERSION_CHECK

// short blip when crossing from "click" to "hold" from off
// (helps the user hit moon mode exactly, instead of holding too long
//  or too short)
#define MOON_TIMING_HINT  // only applies if B_TIMING_ON == B_PRESS_T
// short blips while ramping
#define BLINK_AT_RAMP_MIDDLE
//#define BLINK_AT_RAMP_FLOOR
#define BLINK_AT_RAMP_CEIL
//#define BLINK_AT_STEPS  // whenever a discrete ramp mode is passed in smooth mode

// ramp down via regular button hold if a ramp-up ended <1s ago
// ("hold, release, hold" ramps down instead of up)
// TODO: remove this setting; nobody ever changes it)
#define USE_REVERSING

// Include a simplified UI for non-enthusiasts?
#define USE_SIMPLE_UI

// make the ramps configurable by the user
#define USE_RAMP_CONFIG

// adds a runtime option to switch between automatic memory (default)
// and manual memory (only available if compiled in)
// (manual memory makes 1-click-from-off start at the same level each time)
// (the level can be set explicitly with 5 clicks from on,
// or the user can go back to automatic with click-click-click-click-hold)
// TODO: remap to a shorter button sequence?
#define USE_MANUAL_MEMORY

// enable the battery check mode (shows remaining charge) (requires USE_LVP)
#define USE_BATTCHECK_MODE
// battery readout style (pick one)
// TODO: allow VpT and 4-bar simultaneously,
//       so one can be in "simple mode" and the other in "advanced mode"
#define BATTCHECK_VpT
//#define BATTCHECK_8bars  // FIXME: breaks build
//#define BATTCHECK_4bars  // FIXME: breaks build
// allow the user to calibrate the voltage readings?
// (adjust in 0.05V increments from -0.30V to +0.30V)
// (1 = -0.30V, 2 = -0.25V, ... 7 = 0V, ... 13 = +0.30V)
#define USE_VOLTAGE_CORRECTION

// enable beacon mode
#define USE_BEACON_MODE

// enable sunset timer (ramp-down and automatic shutoff)
// timer is available in regular ramp mode and candle mode
#define USE_SUNSET_TIMER

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
//#define USE_SOS_MODE
//#define USE_SOS_MODE_IN_FF_GROUP  // put SOS in the "boring strobes" mode
//#define USE_SOS_MODE_IN_BLINKY_GROUP  // put SOS in the blinkies mode group

// enable a mode for locking the light for safe carry
#define USE_LOCKOUT_MODE
// should lockout mode function as a momentary moon mode?
#define MOON_DURING_LOCKOUT_MODE
// if enabled, 2nd lockout click goes to the other ramp's floor level
#define LOCKOUT_MOON_FANCY
// add an optional setting to lock the light after being off for a while
#define USE_AUTOLOCK

// enable momentary mode
#define USE_MOMENTARY_MODE

// cut clock speed at very low modes for better efficiency
// (defined here so config files can override it)
#define USE_DYNAMIC_UNDERCLOCKING


#endif
