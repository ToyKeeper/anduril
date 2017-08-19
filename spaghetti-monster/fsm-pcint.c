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
    // debounce a little
    uint8_t highcount = 0;
    // measure for 16/64ths of a ms
    for(uint8_t i=0; i<BP_SAMPLES; i++) {
        // check current value
        uint8_t bit = ((PINB & (1<<SWITCH_PIN)) == 0);
        highcount += bit;
        // wait a moment
        _delay_loop_2(BOGOMIPS/64);
    }
    // use most common value
    uint8_t result = (highcount > (BP_SAMPLES/2));
    //button_was_pressed = result;
    return result;
}

inline void PCINT_on() {
    // enable pin change interrupt for pin N
    GIMSK |= (1 << PCIE);
    // only pay attention to the e-switch pin
    //PCMSK = (1 << SWITCH_PCINT);
    // set bits 1:0 to 0b01 (interrupt on rising *and* falling edge) (default)
    // MCUCR &= 0b11111101;  MCUCR |= 0b00000001;
}

inline void PCINT_off() {
    // disable all pin-change interrupts
    GIMSK &= ~(1 << PCIE);
}

//void button_change_interrupt() {
ISR(PCINT0_vect) {

    //DEBUG_FLASH;

    uint8_t pushed;

    // add event to current sequence
    if (button_is_pressed()) {
        pushed = push_event(A_PRESS);
    } else {
        pushed = push_event(A_RELEASE);
    }

    // check if sequence matches any defined sequences
    // if so, send event to current state callback
    if (pushed) emit_current_event(0);
}

#endif
