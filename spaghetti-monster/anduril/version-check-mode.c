/*
 * version-check-mode.c: Version check mode for Anduril.
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

#ifndef VERSION_CHECK_MODE_C
#define VERSION_CHECK_MODE_C

#include "version-check-mode.h"

// empty state; logic is handled in FSM loop() instead
uint8_t version_check_state(Event event, uint16_t arg) {
    return EVENT_NOT_HANDLED;
}

// this happens in FSM loop()
inline void version_check_iter() {
    for (uint8_t i=0; i<sizeof(version_number)-1; i++) {
        blink_digit(pgm_read_byte(version_number + i) - '0');
        nice_delay_ms(300);
    }
    // FIXME: when user interrupts with button, "off" takes an extra click
    //  before it'll turn back on, because the click to cancel gets sent
    //  to the "off" state instead of version_check_state
    //while (button_is_pressed()) {}
    //empty_event_sequence();

    set_state(off_state, 0);
}


#endif

