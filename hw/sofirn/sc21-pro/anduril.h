// Sofirn SC21 Pro - same setup as a Wurkkos TS10, but with the aux indicator on while ramping
// Copyright (C) 2022-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "wurkkos/ts10/anduril.h"

// turn on the aux LED while main LED is on
#ifdef DEFAULT_AUX_WHILE_ON
#undef DEFAULT_AUX_WHILE_ON
#endif
#define DEFAULT_AUX_WHILE_ON  0b01  // on unless user disables it

