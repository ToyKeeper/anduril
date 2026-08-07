// Sofirn SP36 (small Q8) config options for Anduril using the Attiny1616
// Copyright (C) 2021-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as  the  BLF Q8, mostly
#include "sofirn/blf-q8-t1616/anduril.h"

// voltage readings were a little high with the Q8 value
#undef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 5  // add 0.25V, not 0.35V

// don't blink during the ramp; the button LED brightness is sufficient
// to indicate which power channel(s) are being used
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// stop panicking at ~60% power or ~3000 lm
#ifdef THERM_FASTER_LEVEL
#undef THERM_FASTER_LEVEL
#endif
#define THERM_FASTER_LEVEL 130
