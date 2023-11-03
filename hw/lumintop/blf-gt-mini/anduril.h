// BLF/Lumintop GT Mini config options for Anduril
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "lumintop/blf-gt-mini/hwdef.h"
// Same as an Emisar D1S, except it has a lighted button
#include "hank/emisar-d1s/anduril.h"

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING

// too big, remove stuff to make room
#undef USE_TACTICAL_MODE
#undef USE_SOS_MODE
//#undef USE_RAMP_AFTER_MOON_CONFIG
//#undef USE_RAMP_SPEED_CONFIG
//#undef USE_VOLTAGE_CORRECTION
//#undef USE_2C_STYLE_CONFIG
//#undef USE_TACTICAL_STROBE_MODE

