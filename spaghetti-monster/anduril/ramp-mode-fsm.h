// ramp-mode-fsm.h: FSM config for ramping functions in Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// enable FSM's ramping features
#define USE_RAMPING

// do smooth adjustments when compensating for temperature
#ifdef USE_THERMAL_REGULATION
#define USE_SET_LEVEL_GRADUALLY  // isn't used except for thermal adjustments
#endif

// brightness to use when no memory is set
// FIXME: this is only here to stop an error in fsm-ramping.c,
//        which should be fixed by using a different symbol instead
//        (like BUTTON_LED_BRIGHT_LEVEL or RAMP_HALFWAY_LEVEL or something)
#ifndef DEFAULT_LEVEL
#define DEFAULT_LEVEL MAX_1x7135
#endif

// requires the ability to measure time while "off"
#ifdef USE_MANUAL_MEMORY_TIMER
#define TICK_DURING_STANDBY
#endif

// ensure the jump start feature gets compiled in if needed
#ifdef DEFAULT_JUMP_START_LEVEL
#define USE_JUMP_START
#endif

// include an extra config mode for random stuff which doesn't fit elsewhere
#if defined(USE_JUMP_START) || \
    (defined(USE_CHANNEL_MODE_ARGS) && defined(USE_STEPPED_TINT_RAMPING))
#define USE_GLOBALS_CONFIG
#endif

