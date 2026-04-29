// lockout-mode-levels.h: Level selection helpers for Anduril lockout mode.
// Copyright (C) 2017-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdint.h>

static inline uint8_t lockout_highest_floor(uint8_t floor_a, uint8_t floor_b) {
    if (floor_b > floor_a) return floor_b;
    return floor_a;
}

static inline uint8_t lockout_momentary_level(
        uint8_t click_num,
        uint8_t floor_a,
        uint8_t floor_b,
        uint8_t manual_memory) {
    if ((2 == click_num) && manual_memory) return manual_memory;
    return lockout_highest_floor(floor_a, floor_b);
}
