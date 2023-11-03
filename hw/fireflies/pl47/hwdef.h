// Fireflies PL47 driver layout
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as a D4S, basically, except ...

// ... the PL47 has aux LEDs on pin 7
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB2    // pin 7
#endif

// ... and switch LEDs on pin 3
#ifndef AUXLED2_PIN
#define AUXLED2_PIN   PB4    // pin 3
#endif

// ... and slightly different calibration
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

#include "hank/emisar-d4s/hwdef.h"
#undef FSM_EMISAR_D4S_DRIVER
#undef FSM_EMISAR_D4_DRIVER

