// Fireflies PL47 driver layout
// Copyright (C) 2018-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as a D4S, basically, except ...
#include "hank/emisar-d4s/hwdef.h"

// ... the PL47 has aux LEDs on pin 7
#undef AUX1_LED_PIN
#define AUX1_LED_PIN   PB2    // pin 7

// ... and switch LEDs on pin 3
#define AUX1_LED2_PIN   PB4    // pin 3
#define AUX1_LED2_PORT  PORTB
#define AUX1_LED2_DDR   DDRB

// ... and slightly different calibration
#undef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V

