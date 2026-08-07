// BLF Q8 driver layout
// Copyright (C) 2018-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// Q8 driver is the same as a D4, basically

// ... except the Q8 has a lighted button
#define USE_AUX1_LED
#define USE_AUX1_LED_WHILE_RAMPING
#define AUX1_LED_PIN   PB4    // pin 3
#define AUX1_LED_PORT  PORTB
#define AUX1_LED_DDR   DDRB

// ... and slightly different calibration
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

// the rest is the same as a D4
#include "hank/emisar-d4/hwdef.h"

