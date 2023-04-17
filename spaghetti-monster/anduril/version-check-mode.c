// version-check-mode.c: Version check mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

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

    set_state_deferred(off_state, 0);
}

