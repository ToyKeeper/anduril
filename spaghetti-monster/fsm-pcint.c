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

inline void PCINT_on() {
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85)
        // enable pin change interrupt
        GIMSK |= (1 << PCIE);
        // only pay attention to the e-switch pin
        #if 0  // this is redundant; was already done in main()
        PCMSK = (1 << SWITCH_PCINT);
        #endif
        // set bits 1:0 to 0b01 (interrupt on rising *and* falling edge) (default)
        // MCUCR &= 0b11111101;  MCUCR |= 0b00000001;
    #elif (ATTINY == 1634)
        // enable pin change interrupt
        #ifdef SWITCH2_PCIE
        GIMSK |= ((1 << SWITCH_PCIE) | (1 << SWITCH2_PCIE));
        #else
        GIMSK |= (1 << SWITCH_PCIE);
        #endif
    #elif defined(AVRXMEGA3)  // ATTINY816, 817, etc)
        SWITCH_ISC_REG |= PORT_ISC_BOTHEDGES_gc;
    #else
        #error Unrecognized MCU type
    #endif
}

inline void PCINT_off() {
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85)
        // disable all pin-change interrupts
        GIMSK &= ~(1 << PCIE);
    #elif (ATTINY == 1634)
        // disable all pin-change interrupts
        GIMSK &= ~(1 << SWITCH_PCIE);
    #elif defined(AVRXMEGA3)  // ATTINY816, 817, etc)
        SWITCH_ISC_REG &= ~(PORT_ISC_gm);
    #else
        #error Unrecognized MCU type
    #endif
}

//void button_change_interrupt() {
#if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85) || (ATTINY == 1634)
    #ifdef PCINT_vect
    ISR(PCINT_vect) {
    #else
    ISR(PCINT0_vect) {
    #endif
#elif defined(AVRXMEGA3)  // ATTINY816, 817, etc)
    ISR(SWITCH_VECT) {
        // Write a '1' to clear the interrupt flag
        SWITCH_INTFLG |= (1 << SWITCH_PIN);
#else
    #error Unrecognized MCU type
#endif

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

