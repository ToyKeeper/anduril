// misc.c: Misc extra functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/misc.h"

/* no longer used
void blink_confirm(uint8_t num) {
    uint8_t brightness = actual_level;
    uint8_t bump = actual_level + BLINK_BRIGHTNESS;
    if (bump > MAX_LEVEL) bump = 0;
    for (; num>0; num--) {
        set_level(bump);
        delay_4ms(10/4);
        set_level(brightness);
        if (num > 1) { delay_4ms(100/4); }
    }
}
*/

// make a short, visible pulse
// (either brighter or darker, depending on current brightness)
void blink_once() {
    uint8_t brightness = actual_level;
    uint8_t bump = brightness + BLINK_BRIGHTNESS;
    if (bump > MAX_LEVEL) bump = BLIP_LEVEL;

    set_level(bump);
    delay_4ms(BLINK_ONCE_TIME/4);
    set_level(brightness);
}

// Just go dark for a moment to indicate to user that something happened
void blip() {
    uint8_t temp = actual_level;
    set_level(BLIP_LEVEL);
    delay_4ms(3);
    set_level(temp);
}

