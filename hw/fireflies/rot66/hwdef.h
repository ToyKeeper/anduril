// Fireflies ROT66 driver layout
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as a FW3A, basically, except ...

// ... except the ROT66 has a lighted button
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB2    // pin 7
#endif

// ... and slightly different calibration
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

#include "lumintop/fw3a/hwdef.h"

// ... and no optic nerve
#ifdef VISION_PIN
#undef VISION_PIN
#endif

