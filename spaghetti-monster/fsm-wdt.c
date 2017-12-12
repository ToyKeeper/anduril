/*
 * fsm-wdt.c: WDT (Watch Dog Timer) functions for SpaghettiMonster.
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

#ifndef FSM_WDT_C
#define FSM_WDT_C

#include <avr/interrupt.h>
#include <avr/wdt.h>

void WDT_on()
{
    // interrupt every 16ms
    //cli();                          // Disable interrupts
    wdt_reset();                    // Reset the WDT
    WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
    WDTCR = (1<<WDIE);              // Enable interrupt every 16ms
    //sei();                          // Enable interrupts
}

inline void WDT_off()
{
    //cli();                          // Disable interrupts
    wdt_reset();                    // Reset the WDT
    MCUSR &= ~(1<<WDRF);            // Clear Watchdog reset flag
    WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
    WDTCR = 0x00;                   // Disable WDT
    //sei();                          // Enable interrupts
}

// clock tick -- this runs every 16ms (62.5 fps)
ISR(WDT_vect) {
    // detect and emit button change events
    uint8_t was_pressed = button_last_state;
    uint8_t pressed = button_is_pressed();
    if (was_pressed != pressed) PCINT_inner(pressed);

    //if (ticks_since_last_event < 0xff) ticks_since_last_event ++;
    // increment, but loop from max back to half
    ticks_since_last_event = (ticks_since_last_event + 1) \
                             | (ticks_since_last_event & 0x8000);

    // if time since last event exceeds timeout,
    // append timeout to current event sequence, then
    // send event to current state callback

    // preload recent events
    uint8_t le_num = last_event_num();
    uint8_t last_event = 0;
    uint8_t prev_event = 0;
    if (le_num >= 1) last_event = current_event[le_num-1];
    if (le_num >= 2) prev_event = current_event[le_num-2];

    // callback on each timer tick
    if (last_event == A_HOLD) {
        emit(EV_tick, 0);  // override tick counter while holding button
    }
    else {
        emit(EV_tick, ticks_since_last_event);
    }

    // user held button long enough to count as a long click?
    if (last_event == A_PRESS) {
        if (ticks_since_last_event >= HOLD_TIMEOUT) {
            push_event(A_HOLD);
            emit_current_event(0);
        }
    }

    // user is still holding button, so tick
    else if (last_event == A_HOLD) {
        emit_current_event(ticks_since_last_event);
    }

    // detect completed button presses with expired timeout
    else if (last_event == A_RELEASE) {
        // no timeout required when releasing a long-press
        // TODO? move this logic to PCINT() and simplify things here?
        if (prev_event == A_HOLD) {
            //emit_current_event(0);  // should have been emitted by PCINT
            empty_event_sequence();
        }
        // end and clear event after release timeout
        else if (ticks_since_last_event >= RELEASE_TIMEOUT) {
            push_event(A_RELEASE_TIMEOUT);
            emit_current_event(0);
            empty_event_sequence();
        }
    }

    #if defined(USE_LVP) || defined(USE_THERMAL_REGULATION)
    // start a new ADC measurement every 4 ticks
    static uint8_t adc_trigger = 0;
    adc_trigger ++;
    if (adc_trigger > 3) {
        adc_trigger = 0;
        ADCSRA |= (1 << ADSC) | (1 << ADIE);
    }
    #endif
}

#endif
