// BLF Q8 driver layout using the Attiny1616
// Copyright (C) 2021-2026 gchart, Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * (based on Wurkkos TS10 driver layout,
 *  which in turn was based on an older version of this BLF-Q8-t1616 driver)
 * Driver pinout:
 * eSwitch:    PA5
 * Aux LED:    PB5
 * PWM FET:    PB0 (TCA0 WO0)
 * PWM 1x7135: PB1 (TCA0 WO1)
 * Voltage:    VCC
 */

// identical to TS10 hwdef
#include "wurkkos/ts10/hwdef.h"

