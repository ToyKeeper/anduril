// config-mode.h: Config mode base functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// menus can use 2 colors
#if defined (CONFIG_WAITING_CHANNEL) || defined(CONFIG_BLINK_CHANNEL)
#define USE_CONFIG_COLORS
#endif

// config menu
uint8_t config_state_base(
    Event event,
    uint16_t arg,
    uint8_t num_config_steps,
    void (*savefunc)(uint8_t step, uint8_t value)
    );

