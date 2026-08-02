// Sofirn SP36 (small Q8) config options for Anduril
// Copyright (C) 2018-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as  the  BLF Q8, mostly
#include "sofirn/blf-q8/anduril.h"

// voltage readings were a little high with the Q8 value
#undef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 5  // add 0.25V, not 0.35V

// stop panicking at ~60% power or ~3000 lm
#ifdef THERM_FASTER_LEVEL
#undef THERM_FASTER_LEVEL
#endif
#define THERM_FASTER_LEVEL  130


// AUX + channel modes

// the high button LED mode on this light uses too much power
// off mode: low (1)
// lockout: blinking (3)
#ifdef AUX1_DEFAULT_MODE
#undef AUX1_DEFAULT_MODE
#endif
#define AUX1_DEFAULT_MODE  aux1_cfg_byte(aux_low_e, aux_blinking_e)

