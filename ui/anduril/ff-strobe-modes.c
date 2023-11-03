// ff-strobe-modes.c: Fireflies Flashlights strobe modes for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/ff-strobe-modes.h"

uint8_t boring_strobe_state(Event event, uint16_t arg) {
    // police strobe and SOS, meh
    // 'st' reduces ROM size slightly
    uint8_t st = boring_strobe_type;

    if (event == EV_enter_state) {
        return EVENT_HANDLED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        // reset to police strobe for next time
        boring_strobe_type = 0;
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    // 2 clicks: rotate through strobe/flasher modes
    else if (event == EV_2clicks) {
        boring_strobe_type = (st + 1) % NUM_BORING_STROBES;
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}

inline void boring_strobe_state_iter() {
    switch(boring_strobe_type) {
        #ifdef USE_POLICE_STROBE_MODE
        case 0: // police strobe
            police_strobe_iter();
            break;
        #endif

        #ifdef USE_SOS_MODE_IN_FF_GROUP
        default: // SOS
            sos_mode_iter();
            break;
        #endif
    }
}

#ifdef USE_POLICE_STROBE_MODE
inline void police_strobe_iter() {
    // one iteration of main loop()
    // flash at 16 Hz then 8 Hz, 8 times each
    for (uint8_t del=41; del<100; del+=41) {
        for (uint8_t f=0; f<8; f++) {
            set_level(STROBE_BRIGHTNESS);
            nice_delay_ms(del >> 1);
            set_level(0);
            nice_delay_ms(del);
        }
    }
}
#endif

