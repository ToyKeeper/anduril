// fsm-events.c: Event-handling functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <util/delay_basic.h>


void append_emission(Event event, uint16_t arg) {
    uint8_t i;
    // find last entry
    for(i=0;
        (i<EMISSION_QUEUE_LEN) && (emissions[i].event != EV_none);
        i++) { }
    // add new entry
    if (i < EMISSION_QUEUE_LEN) {
        emissions[i].event = event;
        emissions[i].arg = arg;
    } else {
        // TODO: if queue full, what should we do?
    }
}

void delete_first_emission() {
    uint8_t i;
    for(i=0; i<EMISSION_QUEUE_LEN-1; i++) {
        emissions[i].event = emissions[i+1].event;
        emissions[i].arg = emissions[i+1].arg;
    }
    emissions[i].event = EV_none;
    emissions[i].arg = 0;
}

void process_emissions() {
    while (emissions[0].event != EV_none) {
        emit_now(emissions[0].event, emissions[0].arg);
        delete_first_emission();
    }
}

// Call stacked callbacks for the given event until one handles it.
uint8_t emit_now(Event event, uint16_t arg) {
    for(int8_t i=state_stack_len-1; i>=0; i--) {
        uint8_t err = state_stack[i](event, arg);
        if (! err) return 0;
    }
    return 1;  // event not handled
}

void emit(Event event, uint16_t arg) {
    // add this event to the queue for later,
    // so we won't use too much time during an interrupt
    append_emission(event, arg);
}

void emit_current_event(uint16_t arg) {
    emit(current_event, arg);
}

void empty_event_sequence() {
    current_event = EV_none;
    ticks_since_last_event = 0;
    // when the user completes an input sequence, interrupt any running timers
    // to cancel any delays currently in progress
    // This eliminates a whole bunch of extra code:
    //   before: if (! nice_delay_ms(ms)) {break;}
    //   after: nice_delay_ms(ms);
    interrupt_nice_delays();
}

uint8_t push_event(uint8_t ev_type) {  // only for use by PCINT_inner()
    // don't do this here; do it in PCINT_inner() instead
    //ticks_since_last_event = 0;  // something happened

    // only click events are sent to this function
    current_event |= B_CLICK;

    // handle button presses
    if (ev_type == B_PRESS) {
        // set press flag
        current_event |= B_PRESS;
        // increase click counter
        if ((current_event & B_COUNT) < (B_COUNT)) {
            current_event ++;
        }
        return 1;  // event pushed, even if max clicks already reached
                   // (will just repeat the max over and over)
    }
    // handle button releases
    else if (ev_type == B_RELEASE) {
        // clear the press flag
        current_event &= (~B_PRESS);
        // if a "hold" event just ended, set the timeout flag
        // to indicate that the event is done and can be cleared
        if (current_event & B_HOLD) { current_event |= B_TIMEOUT; }
        return 1;  // event pushed
    }

    return 0;  // unexpected event type
}


// explicitly interrupt these "nice" delays
volatile uint8_t nice_delay_interrupt = 0;
inline void interrupt_nice_delays() { nice_delay_interrupt = 1; }

// like delay_ms, except it aborts on state change
// return value:
//   0: state changed
//   1: normal completion
uint8_t nice_delay_ms(uint16_t ms) {
    /*  // delay_zero() implementation
    if (ms == 0) {
        CLKPR = 1<<CLKPCE; CLKPR = 0;  // full speed
        _delay_loop_2(BOGOMIPS*95/100/3);
        return 1;
    }
    */
    while(ms-- > 0) {
        if (nice_delay_interrupt) {
            return 0;
        }

        #ifdef USE_DYNAMIC_UNDERCLOCKING
        #ifdef USE_RAMPING
        uint8_t level = actual_level;  // volatile, avoid repeat access
        if (level < QUARTERSPEED_LEVEL) {
            clock_prescale_set(clock_div_4);
            _delay_loop_2(BOGOMIPS*DELAY_FACTOR/100/4);
        }
        //else if (level < HALFSPEED_LEVEL) {
        //    clock_prescale_set(clock_div_2);
        //    _delay_loop_2(BOGOMIPS*95/100/2);
        //}
        else {
            clock_prescale_set(clock_div_1);
            _delay_loop_2(BOGOMIPS*DELAY_FACTOR/100);
        }
        // restore regular clock speed
        clock_prescale_set(clock_div_1);
        #else
        // underclock MCU to save power
        clock_prescale_set(clock_div_4);
        // wait
        _delay_loop_2(BOGOMIPS*DELAY_FACTOR/100/4);
        // restore regular clock speed
        clock_prescale_set(clock_div_1);
        #endif  // ifdef USE_RAMPING
        #else
        // wait
        _delay_loop_2(BOGOMIPS*DELAY_FACTOR/100);
        #endif  // ifdef USE_DYNAMIC_UNDERCLOCKING

        // run pending system processes while we wait
        handle_deferred_interrupts();

        // handle events only afterward, so that any collapsed delays will
        // finish running the UI's loop() code before taking any further actions
        // (this helps make sure code runs in the correct order)
        // (otherwise, a new state's EV_enter runs before the old state's
        //  loop() has finished, and things can get weird)
        process_emissions();
    }
    return 1;
}

#ifdef USE_DYNAMIC_UNDERCLOCKING
void delay_4ms(uint8_t ms) {
    while(ms-- > 0) {
        // underclock MCU to save power
        clock_prescale_set(clock_div_4);
        // wait
        _delay_loop_2(BOGOMIPS*98/100);
        // restore regular clock speed
        clock_prescale_set(clock_div_1);
    }
}
#else
void delay_4ms(uint8_t ms) {
    while(ms-- > 0) {
        // wait
        _delay_loop_2(BOGOMIPS*398/100);
    }
}
#endif
/*
uint8_t nice_delay_4ms(uint8_t ms) {
    return nice_delay_ms((uint16_t)ms << 2);
}
*/

/*
uint8_t nice_delay_s() {
    return nice_delay_4ms(250);
}
*/

