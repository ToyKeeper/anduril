/*
 * misc.c: Misc extra functions for Anduril.
 *
 * Copyright (C) 2017 Selene ToyKeeper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MISC_C
#define MISC_C

#include "misc.h"

/* no longer used
void blink_confirm(uint8_t num) {
    uint8_t brightness = actual_level;
    uint8_t bump = actual_level + (MAX_LEVEL/6);
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
    uint8_t bump = brightness + (MAX_LEVEL/6);
    if (bump > MAX_LEVEL) bump = 0;

    set_level(bump);
    delay_4ms(10/4);
    set_level(brightness);
}

// Just go dark for a moment to indicate to user that something happened
void blip() {
    uint8_t temp = actual_level;
    set_level(0);
    delay_4ms(3);
    set_level(temp);
}


#endif

