/*
 * fsm-main.c: main() function for SpaghettiMonster.
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

#ifndef FSM_MAIN_C
#define FSM_MAIN_C

#include "fsm-main.h"

int main() {
    // Don't allow interrupts while booting
    cli();
    //WDT_off();
    //PCINT_off();

    // configure PWM channels
    #if PWM_CHANNELS == 1
    DDRB |= (1 << PWM1_PIN);
    TCCR0B = 0x01; // pre-scaler for timer (1 => 1, 2 => 8, 3 => 64...)
    TCCR0A = PHASE;
    #elif PWM_CHANNELS == 2
    DDRB |= (1 << PWM1_PIN);
    DDRB |= (1 << PWM2_PIN);
    TCCR0B = 0x01; // pre-scaler for timer (1 => 1, 2 => 8, 3 => 64...)
    TCCR0A = PHASE;
    #elif PWM_CHANNELS == 3
    DDRB |= (1 << PWM1_PIN);
    DDRB |= (1 << PWM2_PIN);
    TCCR0B = 0x01; // pre-scaler for timer (1 => 1, 2 => 8, 3 => 64...)
    TCCR0A = PHASE;
    // Second PWM counter is ... weird
    DDRB |= (1 << PWM3_PIN);
    TCCR1 = _BV (CS10);
    GTCCR = _BV (COM1B1) | _BV (PWM1B);
    OCR1C = 255;  // Set ceiling value to maximum
    #elif PWM_CHANNELS == 4
    DDRB |= (1 << PWM1_PIN);
    DDRB |= (1 << PWM2_PIN);
    TCCR0B = 0x01; // pre-scaler for timer (1 => 1, 2 => 8, 3 => 64...)
    TCCR0A = PHASE;
    // Second PWM counter is ... weird
    DDRB |= (1 << PWM3_PIN);
    // FIXME: How exactly do we do PWM on channel 4?
    TCCR1 = _BV (CS10);
    GTCCR = _BV (COM1B1) | _BV (PWM1B);
    OCR1C = 255;  // Set ceiling value to maximum
    DDRB |= (1 << PWM4_PIN);
    #endif

    // TODO: turn on ADC?
    // configure e-switch
    PORTB = (1 << SWITCH_PIN);  // e-switch is the only input
    PCMSK = (1 << SWITCH_PIN);  // pin change interrupt uses this pin

    // TODO: configure sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    // Read config values and saved state
    // restore_state();  // TODO

    // TODO: handle long press vs short press (or even medium press)?

    #ifdef USE_DEBUG_BLINK
    //debug_blink(1);
    #endif

    // all booted -- turn interrupts back on
    PCINT_on();
    WDT_on();
    ADC_on();
    sei();

    // fallback for handling a few things
    push_state(default_state, 0);

    // call recipe's setup
    setup();

    // main loop
    while (1) {
        // TODO: update e-switch press state?
        // TODO: check voltage?
        // TODO: check temperature?
        // if event queue not empty, process and pop first item in queue?
        process_emissions();

        loop();
    }
}

#endif
