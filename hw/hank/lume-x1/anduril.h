// Hank Emisar/Noctigon Lume-X1 config options for Anduril
// Copyright (C) 2018-2024 Selene ToyKeeper, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// For flashlights using the Loneoceans Lume-X1-40W boost driver (AVR32DD20)
// - Same firmware for 6V, 9V, or 12V configs

// same as loneoceans lume-x1-avr32dd20 but with Hank-specific defaults
#include "loneoceans/lume-x1-avr32dd20/anduril.h"
#include "hank/anduril.h"

#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 50

// disable beacontower mode
#ifdef USE_BEACONTOWER_MODE
#undef USE_BEACONTOWER_MODE
#endif

// set smooth ramping by default
#ifdef RAMP_STYLE
#undef RAMP_STYLE
#endif

// reset to anduril default number of steps
#ifdef RAMP_DISCRETE_STEPS
#undef RAMP_DISCRETE_STEPS
#endif
#ifdef SIMPLE_UI_STEPS
#undef SIMPLE_UI_STEPS
#endif
