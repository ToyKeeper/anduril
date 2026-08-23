// channel modes for RGB aux LEDs
// Copyright (C) 2023-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

void set_level_auxred(uint8_t level) {
    set_auxrgb_power(0b000001 << !(!(level)));  // red, high (level > 0) or low
}

void set_level_auxyel(uint8_t level) {
    set_auxrgb_power(0b000101 << !(!(level)));  // red+green, high (level > 0) or low
}

void set_level_auxgrn(uint8_t level) {
    set_auxrgb_power(0b000100 << !(!(level)));  // green, high (level > 0) or low
}

void set_level_auxcyn(uint8_t level) {
    set_auxrgb_power(0b010100 << !(!(level)));  // green+blue, high (level > 0) or low
}

void set_level_auxblu(uint8_t level) {
    set_auxrgb_power(0b010000 << !(!(level)));  // blue, high (level > 0) or low
}

void set_level_auxprp(uint8_t level) {
    set_auxrgb_power(0b010001 << !(!(level)));  // red+blue, high (level > 0) or low
}

void set_level_auxwht(uint8_t level) {
    set_auxrgb_power(0b010101 << !(!(level)));  // red+green+blue, high (level > 0) or low
}

#ifdef USE_AUX_VOLTAGE_CHANNEL_MODE
// defined in anduril/aux-leds.c, which the UI includes after this file;
// returns a ready-made set_auxrgb_power() pattern chosen by battery voltage
uint8_t voltage_to_rgb();

void set_level_auxvoltage(uint8_t level) {
    // same shape as the fixed-color modes above, only the color varies
    set_auxrgb_power(voltage_to_rgb() << !(!(level)));  // high (level > 0) or low
}
#endif

bool gradual_tick_null(uint8_t gt) { return true; }  // do nothing

