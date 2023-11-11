// fsm-pcint.c: PCINT (Pin Change Interrupt) functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <avr/interrupt.h>
#include <util/delay_basic.h>

uint8_t button_is_pressed() {
    uint8_t value = ((SWITCH_PORT & (1<<SWITCH_PIN)) == 0);
    button_last_state = value;
    return value;
}

ISR(SWITCH_VECT) {
    mcu_switch_vect_clear();

    irq_pcint = 1;  // let deferred code know an interrupt happened

    //DEBUG_FLASH;

    // as it turns out, it's more reliable to detect pin changes from WDT
    // because PCINT itself tends to double-tap when connected to a
    // noisy / bouncy switch (so the content of this function has been
    // moved to a separate function, called from WDT only)
    // PCINT_inner(button_is_pressed());
}

// should only be called from PCINT and/or WDT
// (is a separate function to reduce code duplication)
void PCINT_inner(uint8_t pressed) {
    button_last_state = pressed;

    // register the change, and send event to the current state callback
    if (pressed) {  // user pressed button
        push_event(B_PRESS);
        emit_current_event(0);
    } else {  // user released button
        // how long was the button held?
        push_event(B_RELEASE);
        emit_current_event(ticks_since_last_event);
    }
    ticks_since_last_event = 0;
}

