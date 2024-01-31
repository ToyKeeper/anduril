// FireflyLite (Fireflies) config options for Anduril
// Copyright (C) 2018-2023 Selene ToyKeeper, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// For flashlights using the Loneoceans Lume-X1-40W boost driver (ATTINY1616)
// - Models include: X1S, X1L, etc

// same as Loneoceans Lume-X1-40W but with FireflyLite specific config
#include "loneoceans/lume-x1-40w/anduril.h"

#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 50

#undef RAMP_SMOOTH_CEIL
#define RAMP_SMOOTH_CEIL 138    // ~70% ~3000 lumens vs Turbo at ~4000 lumens M4

#undef SIMPLE_UI_CEIL
#define SIMPLE_UI_CEIL 120      // ~2000 lumens
