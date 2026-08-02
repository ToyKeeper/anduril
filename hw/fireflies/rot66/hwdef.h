// Fireflies ROT66 driver layout
// Copyright (C) 2018-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as a FW3A, basically, except ...

// ... except the ROT66 has a lighted button
#define USE_AUX1_LED
#define AUX1_LED_PIN   PB2    // pin 7
#define AUX1_LED_PORT  PORTB
#define AUX1_LED_DDR   DDRB

// ... and slightly different calibration
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

#include "lumintop/fw3a/hwdef.h"

// ... and no optic nerve
#ifdef VISION_PIN
#undef VISION_PIN
#endif

