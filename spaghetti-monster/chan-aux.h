// channel modes for single color aux LEDs
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define NUM_AUX_CHANNEL_MODES  1

// include / exclude field based on compile options
#ifdef USE_CHANNEL_MODE_ARGS
    #define AUX_HAS_ARGS , .has_args = 0
#else
    #define AUX_HAS_ARGS
#endif

#define AUX_CHANNELS \
    { \
        .set_level    = set_level_aux, \
        .gradual_tick = gradual_tick_null \
        AUX_HAS_ARGS \
    }

void set_level_aux(uint8_t level);

bool gradual_tick_null(uint8_t gt);

