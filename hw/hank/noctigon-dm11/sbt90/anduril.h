// Noctigon DM11-SBT90.2 config options for Anduril
// Copyright (C) 2021-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "hank/noctigon-kr4/anduril.h"

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// ... and a single LED in the button
#define USE_BUTTON_LED
// don't use aux LEDs while main LED is on
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

// power channels:
// - linear: 5A?
// - FET: DD

#undef DEFAULT_LEVEL
#define DEFAULT_LEVEL 70

#undef RAMP_SMOOTH_FLOOR
#define RAMP_SMOOTH_FLOOR 10  // low levels may be unreliable
// 10, 30, 50, [70], 90, 110, 130
#undef RAMP_DISCRETE_FLOOR
#define RAMP_DISCRETE_FLOOR 10

// safe limit ~75% power
#undef SIMPLE_UI_CEIL
#define SIMPLE_UI_CEIL RAMP_DISCRETE_CEIL

// stop panicking at ~70% power or ~600 lm
#undef THERM_FASTER_LEVEL
#define THERM_FASTER_LEVEL 130

#undef BLINK_BRIGHTNESS
#define BLINK_BRIGHTNESS 50

#undef CANDLE_AMPLITUDE
#define CANDLE_AMPLITUDE 30

