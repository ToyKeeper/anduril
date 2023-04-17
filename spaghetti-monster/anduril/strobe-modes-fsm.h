// strobe-modes-fsm.h: FSM config for strobe modes in Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// enable the random number generator if we need it
#if defined(USE_LIGHTNING_MODE) || defined(USE_CANDLE_MODE)
#define USE_PSEUDO_RAND
#endif

// party strobe uses really short pulses
#ifdef USE_PARTY_STROBE_MODE
#define USE_DELAY_ZERO
#endif

// candle mode is basically a bunch of stacked random triangle waves
#if defined(USE_CANDLE_MODE)
#define USE_TRIANGLE_WAVE
#endif

// the presence of strobe mode(s) affects how many eeprom bytes we need,
// so it's relevant for FSM configuration
#if defined(USE_CANDLE_MODE) || defined(USE_BIKE_FLASHER_MODE) || defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE) || defined(USE_LIGHTNING_MODE)
#define USE_STROBE_STATE
#endif

