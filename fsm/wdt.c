// fsm-wdt.c: WDT (Watch Dog Timer) functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <avr/interrupt.h>
#include <avr/wdt.h>

// clock tick -- this runs every 16ms (62.5 fps)
ISR(WDT_vect) {
    mcu_wdt_vect_clear();
    irq_wdt = 1;  // WDT event happened
}

void WDT_inner() {
    irq_wdt = 0;  // WDT event handled; reset flag

    static uint8_t adc_trigger = 0;

    // cache this here to reduce ROM size, because it's volatile
    uint16_t ticks_since_last = ticks_since_last_event;
    // increment, but loop from max back to half
    ticks_since_last = (ticks_since_last + 1) \
                     | (ticks_since_last & 0x8000);
    // copy back to the original
    ticks_since_last_event = ticks_since_last;

    // detect and emit button change events (even during standby)
    uint8_t was_pressed = button_last_state;
    uint8_t pressed = button_is_pressed();
    if (was_pressed != pressed) {
        go_to_standby = 0;
        PCINT_inner(pressed);
    }
    // cache again, in case the value changed
    ticks_since_last = ticks_since_last_event;

    #ifdef TICK_DURING_STANDBY
    // handle standby mode specially
    if (go_to_standby) {
        // emit a sleep tick, and process it
        emit(EV_sleep_tick, ticks_since_last);
        process_emissions();

        #ifndef USE_SLEEP_LVP
        return;  // no sleep LVP needed if nothing drains power while off
        #else
        // stop here, usually...  except during the first few seconds asleep, 
        // and once in a while afterward for sleep LVP
        if ((ticks_since_last > (8 * SLEEP_TICKS_PER_SECOND))
            && (0 != (ticks_since_last & 0x0f))) return;

        adc_trigger = 0;  // make sure a measurement will happen
        adc_active_now = 1;  // use ADC noise reduction sleep mode
        ADC_on();  // enable ADC voltage measurement functions temporarily
        #endif
    }
    else {  // button handling should only happen while awake
    #endif

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
        // (first frame of a "hold" event)
        else {
            if (ticks_since_last >= HOLD_TIMEOUT) {
                ticks_since_last_event = 0;
                current_event |= B_HOLD;
                emit_current_event(0);
            }
        }
    }

    // event in progress, but button not currently down
    else if (current_event) {
        // "hold" event just ended
        // no timeout required when releasing a long-press
        if (current_event & B_HOLD) {
            //emit_current_event(ticks_since_last);  // should have been emitted by PCINT_inner()
            empty_event_sequence();
        }
        // end and clear event after release timeout
        else if (ticks_since_last >= RELEASE_TIMEOUT) {
            current_event |= B_TIMEOUT;
            emit_current_event(0);
            empty_event_sequence();
        }
    }

    #ifdef TICK_DURING_STANDBY
    }
    #endif

    #if defined(USE_LVP) || defined(USE_THERMAL_REGULATION)
    // enable the deferred ADC handler once in a while
    if (! adc_trigger) {
        ADC_start_measurement();
        adc_deferred_enable = 1;
    }
    // timing for the ADC handler is every 32 ticks (~2Hz)
    adc_trigger = (adc_trigger + 1) & 31;
    #endif
}

