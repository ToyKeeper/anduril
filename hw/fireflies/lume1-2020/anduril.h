// FireflyLite (Fireflies) config options for Anduril
// Copyright (C) 2018-2023 Selene ToyKeeper, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// for flashlights using the Loneoceans Lume1 driver (ATTINY1634)
// note that some flashlights with single or smaller emitters 
// require the no-fet variant of firmware! e.g. T1R, T9R

// this driver has been superseded by the Lume1-6AF (ATTINY1616)

// same as Loneoceans Lume1-2020 buck driver but with FireflyLite specific config
#include "loneoceans/lume1-2020/anduril.h"

#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 55

#define TURBO_TEMP_EXTRA 7      // in celsius
