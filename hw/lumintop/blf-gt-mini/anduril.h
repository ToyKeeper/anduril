// BLF/Lumintop GT Mini config options for Anduril
// Copyright (C) 2018-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// Same as an Emisar D1S, except it has a lighted button
#include "hank/emisar-d1s/anduril.h"
#undef HWDEF_H
#define HWDEF_H  lumintop/blf-gt-mini/hwdef.h

// the button lights up
// the button is visible while main LEDs are on
#define USE_AUX1_LED_WHILE_RAMPING

// too big, remove stuff to make room
#undef USE_TACTICAL_MODE
#undef USE_SOS_MODE
//#undef USE_RAMP_AFTER_MOON_CONFIG
//#undef USE_RAMP_SPEED_CONFIG
//#undef USE_VOLTAGE_CORRECTION
//#undef USE_2C_STYLE_CONFIG
//#undef USE_TACTICAL_STROBE_MODE

