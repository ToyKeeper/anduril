// ff-strobe-modes.h: Fireflies Flashlights strobe modes for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

uint8_t boring_strobe_state(Event event, uint16_t arg);
inline void boring_strobe_state_iter();
uint8_t boring_strobe_type = 0;
void sos_blink(uint8_t num, uint8_t dah);
#ifdef USE_POLICE_STROBE_MODE
inline void police_strobe_iter();
#endif
#define NUM_BORING_STROBES 2

