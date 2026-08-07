// Emisar D4S driver layout
// Copyright (C) 2018-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as a D4, basically
#include "hank/emisar-d4/hwdef.h"

// ... except the D4S has aux LEDs under the optic
#ifndef USE_AUX1_LED
#define USE_AUX1_LED
//#define USE_AUX1_LED_WHILE_RAMPING
#define AUX1_LED_PIN   PB4    // pin 3
#define AUX1_LED_PORT  PORTB
#define AUX1_LED_DDR   DDRB
#endif

