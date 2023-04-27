// tactical-mode.h: Tactical mode for Anduril.
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifndef TACTICAL_LEVELS
    // high, low, tactical strobe
    #ifdef USE_POLICE_COLOR_STROBE_MODE
        #define TACTICAL_LEVELS 120,30,(RAMP_SIZE+3)
    #else
        #define TACTICAL_LEVELS 120,30,(RAMP_SIZE+2)
    #endif
#endif

// tactical(ish) mode
uint8_t tactical_state(Event event, uint16_t arg);
uint8_t tactical_config_state(Event event, uint16_t arg);

