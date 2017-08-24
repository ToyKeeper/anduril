/*
 * fsm-events.c: Event-handling functions for SpaghettiMonster.
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

#ifndef FSM_EVENTS_C
#define FSM_EVENTS_C

// TODO: maybe compare events by number instead of pointer?
//       (number = index in event types array)
//       (comparison would use full event content, but send off index to callbacks)
//       (saves space by using uint8_t instead of a pointer)
//       (also eliminates the need to duplicate single-entry events like for voltage or timer tick)

// return 1 if (a == b), 0 otherwise
uint8_t compare_event_sequences(uint8_t *a, const uint8_t *b) {
    for(uint8_t i=0; (i<EV_MAX_LEN) && (a[i] == pgm_read_byte(b+i)); i++) {
        // end of zero-terminated sequence
        if (a[i] == 0) return 1;
    }
    // if we ever fall out, that means something was different
    // (or the sequence is too long)
    return 0;
}

void empty_event_sequence() {
    for(uint8_t i=0; i<EV_MAX_LEN; i++) current_event[i] = 0;
}

uint8_t push_event(uint8_t ev_type) {
    ticks_since_last_event = 0;  // something happened
    uint8_t i;
    uint8_t prev_event = 0;  // never push the same event twice in a row
    for(i=0; current_event[i] && (i<EV_MAX_LEN); i++)
        prev_event = current_event[i];
    if ((i < EV_MAX_LEN)  &&  (prev_event != ev_type)) {
        current_event[i] = ev_type;
        return 1;  // event pushed
    } else {
        // TODO: ... something?
    }
    return 0;  // no event pushed
}

// find and return last action in the current event sequence
/*
uint8_t last_event(uint8_t offset) {
    uint8_t i;
    for(i=0; current_event[i] && (i<EV_MAX_LEN); i++);
    if (i == EV_MAX_LEN) return current_event[EV_MAX_LEN-offset];
    else if (i >= offset) return current_event[i-offset];
    return 0;
}
*/

inline uint8_t last_event_num() {
    uint8_t i;
    for(i=0; current_event[i] && (i<EV_MAX_LEN); i++);
    return i;
}


void append_emission(EventPtr event, uint16_t arg) {
    uint8_t i;
    // find last entry
    for(i=0;
        (i<EMISSION_QUEUE_LEN) && (emissions[i].event != NULL);
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
    emissions[i].event = NULL;
    emissions[i].arg = 0;
}

void process_emissions() {
    while (emissions[0].event != NULL) {
        emit_now(emissions[0].event, emissions[0].arg);
        delete_first_emission();
    }
}

// like delay_ms, except it aborts on state change
// return value:
//   0: state changed
//   1: normal completion
uint8_t nice_delay_ms(uint16_t ms) {
    StatePtr old_state = current_state;
    while(ms-- > 0) {
        _delay_loop_2(BOGOMIPS*98/100);
        process_emissions();
        if (old_state != current_state) {
            return 0;  // state changed; abort
        }
    }
    return 1;
}

// Call stacked callbacks for the given event until one handles it.
uint8_t emit_now(EventPtr event, uint16_t arg) {
    for(int8_t i=state_stack_len-1; i>=0; i--) {
        uint8_t err = state_stack[i](event, arg);
        if (! err) return 0;
    }
    return 1;  // event not handled
}

void emit(EventPtr event, uint16_t arg) {
    // add this event to the queue for later,
    // so we won't use too much time during an interrupt
    append_emission(event, arg);
}

// Search the pre-defined event list for one matching what the user just did,
// and emit it if one was found.
void emit_current_event(uint16_t arg) {
    //uint8_t err = 1;
    for (uint8_t i=0; i<(sizeof(event_sequences)/sizeof(EventPtr)); i++) {
        if (events_match(current_event, event_sequences[i])) {
            //DEBUG_FLASH;
            //err = emit(event_sequences[i], arg);
            //return err;
            emit(event_sequences[i], arg);
            return;
        }
    }
    //return err;
}

// TODO? add events to a queue when inside an interrupt
//       instead of calling the event functions directly?
//       (then empty the queue in main loop?)

#endif
