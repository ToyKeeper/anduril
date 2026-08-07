// Wurkkos TS11, like a TS25  but with a RGB button and RGB front aux
// Copyright (C) 2023-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "wurkkos/ts25/anduril.h"

// turn on the aux LEDs while main LEDs are on
// (but not until the main LEDs are bright enough to overpower the aux)
// (setting this lower makes an annoying effect on some levels)
#define USE_AUXRGB_LEDS_WHILE_ON  50
#undef DEFAULT_AUX_WHILE_ON
#define DEFAULT_AUX_WHILE_ON  0b10  // RGB aux only

