// Emisar D4S driver layout
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as a D4, basically
#include "hwdef-emisar-d4.h"

// ... except the D4S has aux LEDs under the optic
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB4    // pin 3
#endif

