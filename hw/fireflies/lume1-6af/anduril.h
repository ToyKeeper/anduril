// FireflyLite (Fireflies) config options for Anduril
// Copyright (C) 2018-2024 Selene ToyKeeper, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// For flashlights using the Loneoceans Lume1-6AF driver (ATTINY1616)
// - NOTE: some flashlights with single or smaller emitters require the no-fet variant of firmware!
// - Models include: E07X Pro, E12R, X1S, X1L, NOV-Mu, T1R, T9R

// same as Loneoceans Lume1-6AF buck driver but with FireflyLite specific config
#include "loneoceans/lume1-6af/anduril.h"

#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 50

#undef THERM_RESPONSE_MAGNITUDE
#define THERM_RESPONSE_MAGNITUDE 64

#undef RAMP_SMOOTH_FLOOR
#define RAMP_SMOOTH_FLOOR 9
