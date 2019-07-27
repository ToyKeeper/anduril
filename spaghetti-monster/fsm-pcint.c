/*
 * fsm-pcint.c: PCINT (Pin Change Interrupt) functions for SpaghettiMonster.
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

#ifndef FSM_PCINT_C
#define FSM_PCINT_C

#include <avr/interrupt.h>
#include <util/delay_basic.h>

uint8_t button_is_pressed() {
    // remember the past 32 measurements
    static uint32_t readings = 0;
    // take at least one new measurement,
    // and wait for measurements to settle to all zeroes or all ones
    do {
        // shift past readings and add current value
        readings = (readings << 1) | ((SWITCH_PORT & (1<<SWITCH_PIN)) == 0);
        // wait a moment
        _delay_loop_2(BOGOMIPS/16);  // up to 2ms to stabilize
    }
    while ((readings != 0) && (readings != 0xFFFFFFFF));
    button_last_state = readings;
    return readings;
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
    #else
        #error Unrecognized MCU type
    #endif
}

//void button_change_interrupt() {
#if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85) || (ATTINY == 1634)
EMPTY_INTERRUPT(PCINT0_vect);
#else
    #error Unrecognized MCU type
#endif
/*
ISR(PCINT0_vect) {

    //DEBUG_FLASH;

    // as it turns out, it's more reliable to detect pin changes from WDT
    // because PCINT itself tends to double-tap when connected to a
    // noisy / bouncy switch (so the content of this function has been
    // moved to a separate function, called from WDT only)
    // PCINT_inner(button_is_pressed());

}
*/

// should only be called from PCINT and/or WDT
// (is a separate function to reduce code duplication)
void PCINT_inner(uint8_t pressed) {
    uint8_t pushed;

    if (pressed) {
        pushed = push_event(B_PRESS);
    } else {
        pushed = push_event(B_RELEASE);
    }

    // check if sequence matches any defined sequences
    // if so, send event to current state callback
    if (pushed) {
        button_last_state = pressed;
        emit_current_event(0);
    }
}
#endif
