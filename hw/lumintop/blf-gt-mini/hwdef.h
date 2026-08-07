// BLF/Lumintop GT Mini driver layout
// Copyright (C) 2018-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// exactly the same as a D1S, but with a lighted button
// (which is the same hwdef as a D4)
#include "hank/emisar-d4/hwdef.h"

// lighted button
#define USE_AUX1_LED
#define AUX1_LED_PIN   PB4    // pin 3
#define AUX1_LED_PORT  PORTB
#define AUX1_LED_DDR   DDRB

