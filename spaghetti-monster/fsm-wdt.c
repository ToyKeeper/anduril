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
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85)
        // interrupt every 16ms
        //cli();                          // Disable interrupts
        wdt_reset();                    // Reset the WDT
        WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
        WDTCR = (1<<WDIE);              // Enable interrupt every 16ms
        //sei();                          // Enable interrupts
    #elif (ATTINY == 1634)
        wdt_reset();                    // Reset the WDT
        WDTCSR = (1<<WDIE);             // Enable interrupt every 16ms
    #else
        #error Unrecognized MCU type
    #endif
}

#ifdef TICK_DURING_STANDBY
inline void WDT_slow()
{
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85)
        // interrupt slower
        //cli();                          // Disable interrupts
        wdt_reset();                    // Reset the WDT
        WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
        WDTCR = (1<<WDIE) | STANDBY_TICK_SPEED; // Enable interrupt every so often
        //sei();                          // Enable interrupts
    #elif (ATTINY == 1634)
        wdt_reset();                    // Reset the WDT
        WDTCSR = (1<<WDIE) | STANDBY_TICK_SPEED;
    #else
        #error Unrecognized MCU type
    #endif
}
#endif

inline void WDT_off()
{
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85)
        //cli();                          // Disable interrupts
        wdt_reset();                    // Reset the WDT
        MCUSR &= ~(1<<WDRF);            // Clear Watchdog reset flag
        WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
        WDTCR = 0x00;                   // Disable WDT
        //sei();                          // Enable interrupts
    #elif (ATTINY == 1634)
        cli();                // needed because CCP, below
        wdt_reset();          // Reset the WDT
        MCUSR &= ~(1<<WDRF);  // clear watchdog reset flag
        CCP = 0xD8;           // enable config changes
        WDTCSR = 0;           // disable and clear all WDT settings
        sei();
    #else
        #error Unrecognized MCU type
    #endif
}

// clock tick -- this runs every 16ms (62.5 fps)
ISR(WDT_vect) {
    irq_wdt = 1;  // WDT event happened
}

void WDT_inner() {
    irq_wdt = 0;  // WDT event handled; reset flag

    static uint8_t adc_trigger = 0;

    // detect and emit button change events (even during standby)
    uint8_t was_pressed = button_last_state;
    uint8_t pressed = button_is_pressed();
    if (was_pressed != pressed) {
        go_to_standby = 0;
        PCINT_inner(pressed);
    }

    #ifdef TICK_DURING_STANDBY
    static uint16_t sleep_counter = 0;
    // handle standby mode specially
    if (go_to_standby) {
        // emit a halfsleep tick, and process it
        emit(EV_sleep_tick, sleep_counter);
        // wrap around from 65535 to 32768, not 0
        sleep_counter = (sleep_counter + 1) | (sleep_counter & 0x8000);
        process_emissions();

        #if defined(USE_SLEEP_LVP)
        // stop here, usually...  but proceed often enough for sleep LVP to work
        if (0 != (sleep_counter & 0x7f)) return;
        adc_trigger = 255;  // make sure a measurement will happen
        #else
        return;  // no sleep LVP needed if nothing drains power while off
        #endif
    }
    else { sleep_counter = 0; }
    #endif

    // cache this here to reduce ROM size, because it's volatile
    uint16_t ticks_since_last = ticks_since_last_event;
 
    // increment, but loop from max back to half
    //if (ticks_since_last < 0xff) ticks_since_last ++;
    ticks_since_last = (ticks_since_last + 1) \
                     | (ticks_since_last & 0x8000);
    // copy back to the original
    ticks_since_last_event = ticks_since_last;

    // if time since last event exceeds timeout,
    // append timeout to current event sequence, then
    // send event to current state callback

    // callback on each timer tick
    if ((current_event & B_FLAGS) == (B_CLICK | B_HOLD | B_PRESS)) {
        emit(EV_tick, 0);  // override tick counter while holding button
    }
    else {
        emit(EV_tick, ticks_since_last);
    }

    // user held button long enough to count as a long click?
    if (current_event & B_PRESS) {
        // during a "hold", send a hold event each tick, with a timer
        if (current_event & B_HOLD) {
            emit_current_event(ticks_since_last);
        }
        // has button been down long enough to become a "hold"?
        else {
            if (ticks_since_last >= HOLD_TIMEOUT) {
                //ticks_since_last_event = 0;
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
        else if (ticks_since_last >= RELEASE_TIMEOUT) {
            current_event |= B_TIMEOUT;
            //ticks_since_last_event = 0;
            emit_current_event(0);
            empty_event_sequence();
        }
    }

    #if defined(USE_LVP) || defined(USE_THERMAL_REGULATION)
    // start a new ADC measurement every 4 ticks
    adc_trigger ++;
    if (0 == (adc_trigger & 3)) {
        #if defined(TICK_DURING_STANDBY) && defined(USE_SLEEP_LVP)
        // we shouldn't be here unless it woke up for a LVP check...
        // so enable ADC voltage measurement functions temporarily
        if (go_to_standby) ADC_on();
        #endif
        ADC_start_measurement();
        irq_adc = 0;
        adcint_enable = 1;
    }
    #endif
}

#endif
