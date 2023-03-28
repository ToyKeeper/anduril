/*
 * momentary-mode.c: Momentary mode for Anduril.
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

#ifndef MOMENTARY_MODE_C
#define MOMENTARY_MODE_C

#include "momentary-mode.h"

uint8_t momentary_state(Event event, uint16_t arg) {
    // init strobe mode, if relevant
    #ifdef USE_STROBE_STATE
    if ((event == EV_enter_state) && (momentary_mode != 0)) {
        strobe_state(event, arg);
    }
    #endif

    // light up when the button is pressed; go dark otherwise
    // button is being held
    if ((event & (B_CLICK | B_PRESS)) == (B_CLICK | B_PRESS)) {
        momentary_active = 1;
        // 0 = ramping, 1 = strobes
        if (momentary_mode == 0) {
            set_level(memorized_level);
        }
        return MISCHIEF_MANAGED;
    }
    // button was released
    else if ((event & (B_CLICK | B_PRESS)) == (B_CLICK)) {
        momentary_active = 0;
        set_level(0);
        //go_to_standby = 1;  // sleep while light is off
        return MISCHIEF_MANAGED;
    }

    // Sleep, dammit!  (but wait a few seconds first)
    // (because standby mode uses such little power that it can interfere
    //  with exiting via tailcap loosen+tighten unless you leave power
    //  disconnected for several seconds, so we want to be awake when that
    //  happens to speed up the process)
    else if (event == EV_tick) {
        #ifdef USE_STROBE_STATE
        if (momentary_active) {
            // 0 = ramping, non-zero = strobes
            if (momentary_mode != 0) {
                return strobe_state(event, arg);
            }
        }
        else {
        #endif
            if (arg > TICKS_PER_SECOND*5) {  // sleep after 5 seconds
                go_to_standby = 1;  // sleep while light is off
                // turn off lighted button
                #ifdef USE_INDICATOR_LED
                indicator_led(0);
                #elif defined(USE_AUX_RGB_LEDS)
                rgb_led_update(0, 0);
                #endif
            }
        #ifdef USE_STROBE_STATE
        }
        #endif
        return MISCHIEF_MANAGED;
    }

    return EVENT_NOT_HANDLED;
}


#endif

