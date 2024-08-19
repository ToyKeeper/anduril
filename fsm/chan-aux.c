// channel modes for single color aux LEDs
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

void set_level_aux(uint8_t level) {
    indicator_led((!(!(level)) << 1) + 1);  // high (level > 0) or low
}

bool gradual_tick_null(uint8_t gt) { return true; }  // do nothing

