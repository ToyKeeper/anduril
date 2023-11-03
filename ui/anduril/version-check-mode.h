// version-check-mode.h: Version check mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define USE_BLINK_DIGIT  // FIXME: does nothing unless defined earlier

#ifndef MODEL_NUMBER
// if no model number, it's a build error
//#define MODEL_NUMBER "0000"
#error MODEL_NUMBER undefined
#endif

#include "anduril/version.h"
const PROGMEM uint8_t version_number[] = MODEL_NUMBER "." VERSION_NUMBER;
uint8_t version_check_state(Event event, uint16_t arg);
inline void version_check_iter();

