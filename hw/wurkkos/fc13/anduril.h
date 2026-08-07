// Wurkkos FC13, like a TS25 but with a RGB button and no front aux
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "wurkkos/ts25/anduril.h"

// turn on the aux LEDs (RGB button) while main LEDs are on
#define USE_AUXRGB_LEDS_WHILE_ON  20
#undef DEFAULT_AUX_WHILE_ON
#define DEFAULT_AUX_WHILE_ON  0b10  // RGB aux only
