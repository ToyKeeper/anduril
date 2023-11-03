// BLF/Lumintop GT Mini driver layout
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// exactly the same as a D1S, but with a lighted button
// (which is the same hwdef as a D4)
#include "hank/emisar-d4/hwdef.h"

// lighted button
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB4    // pin 3
#endif

