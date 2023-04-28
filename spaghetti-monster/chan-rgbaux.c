// channel modes for RGB aux LEDs
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

void set_level_auxred(uint8_t level) {
    rgb_led_set(!(!(level)) << 1);  // red, high (or off)
}

void set_level_auxgrn(uint8_t level) {
    rgb_led_set(!(!(level)) << 3);  // green, high (or off)
}

void set_level_auxblu(uint8_t level) {
    rgb_led_set(!(!(level)) << 5);  // blue, high (or off)
}

bool gradual_tick_null(uint8_t gt) { return true; }  // do nothing

