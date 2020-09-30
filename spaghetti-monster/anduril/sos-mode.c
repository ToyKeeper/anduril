/*
 * sos-mode.c: SOS mode for Anduril.
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

#ifndef SOS_MODE_C
#define SOS_MODE_C

#include "sos-mode.h"

#ifdef USE_SOS_MODE_IN_BLINKY_GROUP
uint8_t sos_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: next blinky mode
    else if (event == EV_2clicks) {
        #if defined(USE_BATTCHECK_MODE)
        set_state(battcheck_state, 0);
        #elif defined(USE_THERMAL_REGULATION)
        set_state(tempcheck_state, 0);
        #elif defined(USE_BEACON_MODE)
        set_state(beacon_state, 0);
        #endif
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}
#endif

void sos_blink(uint8_t num, uint8_t dah) {
    #define DIT_LENGTH 200
    for (; num > 0; num--) {
        set_level(memorized_level);
        nice_delay_ms(DIT_LENGTH);
        if (dah) {  // dah is 3X as long as a dit
            nice_delay_ms(DIT_LENGTH*2);
        }
        set_level(0);
        // one "off" dit between blinks
        nice_delay_ms(DIT_LENGTH);
    }
    // three "off" dits (or one "dah") between letters
    // (except for SOS, which is collectively treated as a single "letter")
    //nice_delay_ms(DIT_LENGTH*2);
}

inline void sos_mode_iter() {
    // one iteration of main loop()
    //nice_delay_ms(1000);
    sos_blink(3, 0);  // S
    sos_blink(3, 1);  // O
    sos_blink(3, 0);  // S
    nice_delay_ms(2000);
}


#endif

