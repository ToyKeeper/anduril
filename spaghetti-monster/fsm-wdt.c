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

#ifdef TICK_DURING_STANDBY
inline void WDT_slow()
{
    // interrupt slower
    //cli();                          // Disable interrupts
    wdt_reset();                    // Reset the WDT
    WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
    WDTCR = (1<<WDIE) | STANDBY_TICK_SPEED; // Enable interrupt every so often
    //sei();                          // Enable interrupts
}
#endif

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
    #ifdef TICK_DURING_STANDBY
    f_wdt = 1;  // WDT event happened

    static uint16_t sleep_counter = 0;
    // handle standby mode specially
    if (go_to_standby) {
        // emit a halfsleep tick, and process it
        emit(EV_sleep_tick, sleep_counter++);
        process_emissions();
        return;
    }
    sleep_counter = 0;
    #endif

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

    // callback on each timer tick
    if ((current_event & B_FLAGS) == (B_HOLD | B_PRESS)) {
        emit(EV_tick, 0);  // override tick counter while holding button
    }
    else {
        emit(EV_tick, ticks_since_last_event);
    }

    // user held button long enough to count as a long click?
    if (current_event & B_PRESS) {
        // during a "hold", send a hold event each tick, with a timer
        if (current_event & B_HOLD) {
            emit_current_event(ticks_since_last_event);
        }
        // has button been down long enough to become a "hold"?
        else {
            if (ticks_since_last_event >= HOLD_TIMEOUT) {
                current_event |= B_HOLD;
                emit_current_event(0);
            }
        }
    }

    // event in progress, but button not currently down
    else if (current_event) {
        // "hold" event just ended
        // no timeout required when releasing a long-press
        // TODO? move this logic to PCINT() and simplify things here?
        if (current_event & B_HOLD) {
            //emit_current_event(0);  // should have been emitted by PCINT
            empty_event_sequence();
        }
        // end and clear event after release timeout
        else if (ticks_since_last_event >= RELEASE_TIMEOUT) {
            current_event |= B_TIMEOUT;
            emit_current_event(0);
            empty_event_sequence();
        }
    }

    #if defined(USE_LVP) || defined(USE_THERMAL_REGULATION)
    // start a new ADC measurement every 4 ticks
    static uint8_t adc_trigger = 0;
    adc_trigger ++;
    if (0 == (adc_trigger & 3)) {
        ADCSRA |= (1 << ADSC) | (1 << ADIE);
        adcint_enable = 1;
    }
    #endif
}

#endif
