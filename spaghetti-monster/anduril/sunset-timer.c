/*
 * sunset-timer.c: Sunset / candle auto-shutoff functions for Anduril.
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

#ifndef SUNSET_TIMER_C
#define SUNSET_TIMER_C

#include "sunset-timer.h"

uint8_t sunset_timer_state(Event event, uint16_t arg) {

    // blink on start
    if (event == EV_enter_state) {
        sunset_timer = 0;
        sunset_ticks = 0;
        return MISCHIEF_MANAGED;
    }
    // 4H: add 10m to timer, per second, until released
    else if (event == EV_click4_hold) {
        if (0 == (arg % TICKS_PER_SECOND)) {
            if (sunset_timer < (255 - SUNSET_TIMER_UNIT)) {
                // add 30m to the timer
                sunset_timer += SUNSET_TIMER_UNIT;
                sunset_timer_peak = sunset_timer;  // reset ceiling
                sunset_ticks = 0;  // reset phase
                // blink to confirm
                uint8_t brightness = actual_level;
                uint8_t bump = actual_level + 32;
                if (bump > MAX_LEVEL) bump = 0;
                set_level(bump);
                delay_4ms(2);
                set_level(brightness);
            }
        }
        return MISCHIEF_MANAGED;
    }
    // tick: step down (maybe) or off (maybe)
    else if (event == EV_tick) {
        // time passed
        sunset_ticks ++;
        // did we reach a minute mark?
        if (sunset_ticks >= TICKS_PER_MINUTE) {
            sunset_ticks = 0;
            if (sunset_timer > 0) {
                sunset_timer --;
            }
        }
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}


#endif

