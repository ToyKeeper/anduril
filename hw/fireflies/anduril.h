// Fireflies brand default config options for Anduril
// Copyright (C) 2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// Fireflies likes rainbow mode by default, to impress new customers
// (people usually change it to voltage mode later though)
#ifdef RGB_LED_OFF_DEFAULT
#undef RGB_LED_OFF_DEFAULT
#endif
#ifdef RGB_LED_LOCKOUT_DEFAULT
#undef RGB_LED_LOCKOUT_DEFAULT
#endif
#define RGB_LED_OFF_DEFAULT \
    auxrgb_cfg_byte(aux_low_e, aux_rgb_rainbow_e)
#define RGB_LED_LOCKOUT_DEFAULT \
    auxrgb_cfg_byte(aux_blinking_e, aux_rgb_voltage_e)

// maybe disable POVD by default since it confuses some customers
//#define DEFAULT_POST_OFF_VOLTAGE_SECONDS  0

// Allow 3C (or 6C) in Simple UI (toggle smooth or stepped ramping)
#define USE_SIMPLE_UI_RAMPING_TOGGLE

// allow Aux Config and Strobe Modes in Simple UI
#define USE_EXTENDED_SIMPLE_UI

// Fireflies likes stepped ramp by default
#ifdef RAMP_STYLE
#undef RAMP_STYLE
#endif
#define RAMP_STYLE  1

// double click while on goes to full-power turbo, not ramp ceiling
#define DEFAULT_2C_STYLE 1

// Fireflies users expect Beacontower blinky mode to exist
#define USE_BEACONTOWER_MODE

// doing "loosen, press, tighten, hold" is hard on many lights
// so also do factory reset on "Off -> 13H"
#define USE_SOFT_FACTORY_RESET

