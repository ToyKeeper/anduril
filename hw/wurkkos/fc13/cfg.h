// Wurkkos FC13, like a TS25 but with a RGB button and no front aux
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// ATTINY: 1616
#include "cfg-wurkkos-ts25.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0716"

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// turn on the aux LEDs while main LEDs are on
#define USE_AUX_RGB_LEDS_WHILE_ON  20
#define USE_INDICATOR_LED_WHILE_RAMPING

