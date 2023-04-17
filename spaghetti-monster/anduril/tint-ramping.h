// tint-ramping.h: Tint ramping functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// 0: smooth tint ramp
// 1: toggle tint only between two extremes
#ifdef TINT_RAMP_TOGGLE_ONLY
uint8_t tint_style = 1;
#else
uint8_t tint_style = 0;
#endif

#ifdef USE_MANUAL_MEMORY
uint8_t manual_memory_tint;
#endif

// not actually a mode, more of a fallback under other modes
uint8_t tint_ramping_state(Event event, uint16_t arg);

