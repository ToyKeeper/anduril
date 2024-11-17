// FireflyLite (Fireflies) config options for Anduril
// Copyright (C) 2018-2024 Selene ToyKeeper, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// For flashlights using the Loneoceans Lume-X1-40W boost driver (ATTINY1616)
// - Models include: X1S, X1L, etc

// same as Loneoceans Lume-X1-40W but with FireflyLite specific config
#include "loneoceans/lume-x1-40w/anduril.h"

#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 50

#undef RAMP_SMOOTH_CEIL
#define RAMP_SMOOTH_CEIL 125    // ~18-20W (3.0A)

#undef SIMPLE_UI_CEIL
#define SIMPLE_UI_CEIL 113      // ~12W (2.0A)
