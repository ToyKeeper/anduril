/*
 * cfg-default.h: Default configuration for Anduril.
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

#ifndef CFG_DEFAULT_H
#define CFG_DEFAULT_H

/********* User-configurable options *********/
#define USE_LVP

// parameters for this defined below or per-driver
#define USE_THERMAL_REGULATION
#define DEFAULT_THERM_CEIL 45  // try not to get hotter than this

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
#define USE_REVERSING

// add a runtime option to switch between automatic memory (default)
// and manual memory (only available if compiled in)
// (manual memory makes 1-click-from-off start at the same level each time)
// (the level can be set explicitly with 5 clicks from on,
// or the user can go back to automatic with click-click-click-click-hold)
#define USE_MANUAL_MEMORY

// battery readout style (pick one)
#define BATTCHECK_VpT
//#define BATTCHECK_8bars  // FIXME: breaks build
//#define BATTCHECK_4bars  // FIXME: breaks build

// enable/disable various strobe modes
#define USE_BIKE_FLASHER_MODE
#define USE_PARTY_STROBE_MODE
#define USE_TACTICAL_STROBE_MODE
#define USE_LIGHTNING_MODE
#define USE_CANDLE_MODE

// enable sunset (goodnight) mode
#define USE_GOODNIGHT_MODE
#define GOODNIGHT_TIME  60  // minutes (approximately)
#define GOODNIGHT_LEVEL 24  // ~11 lm

// enable beacon mode
#define USE_BEACON_MODE

// enable momentary mode
#define USE_MOMENTARY_MODE

// Include a simplified UI for non-enthusiasts?
#define USE_SIMPLE_UI

// make the ramps configurable by the user
#define USE_RAMP_CONFIG

// boring strobes nobody really likes, but sometimes flashlight companies want
// (these replace the fun strobe group,
//  so don't enable them at the same time as any of the above strobes)
//#define USE_POLICE_STROBE_MODE
//#define USE_SOS_MODE
//#define USE_SOS_MODE_IN_FF_GROUP  // put SOS in the "boring strobes" mode
//#define USE_SOS_MODE_IN_BLINKY_GROUP  // put SOS in the blinkies mode group

// cut clock speed at very low modes for better efficiency
// (defined here so config files can override it)
#define USE_DYNAMIC_UNDERCLOCKING


#endif
