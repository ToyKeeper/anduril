/*
 * channel-modes.h: Multi-channel functions for Anduril.
 * Copyright (C) 2017-2023 Selene ToyKeeper
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#if defined(USE_MANUAL_MEMORY) && defined(USE_CHANNEL_MODE_ARGS)
// TODO: save to eeprom
// remember and reset 1 extra parameter per channel mode (like tint)
uint8_t manual_memory_channel_args[NUM_CHANNEL_MODES] = { CHANNEL_MODE_ARGS };
#endif

// not actually a mode, more of a fallback under other modes
uint8_t channel_mode_state(Event event, uint16_t arg);

#if NUM_CHANNEL_MODES > 1
uint8_t channel_mode_config_state(Event event, uint16_t arg);
#endif
