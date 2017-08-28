/*
 * fsm-states.c: State-handling functions for SpaghettiMonster.
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
 
#ifndef FSM_STATES_C
#define FSM_STATES_C

#include "fsm-states.h"
#include "fsm-adc.h"

// TODO: if callback doesn't handle current event,
//       pass event to next state on stack?
//       Callback return values:
//       0: event handled normally
//       1: event not handled
//       255: error (not sure what this would even mean though, or what difference it would make)
// TODO: function to call stacked callbacks until one returns "handled"

void _set_state(StatePtr new_state, uint16_t arg,
                EventPtr exit_event, EventPtr enter_event) {
    // call old state-exit hook (don't use stack)
    if (current_state != NULL) current_state(exit_event, arg);
    // set new state
    current_state = new_state;
    // call new state-enter hook (don't use stack)
    if (new_state != NULL) current_state(enter_event, arg);
}

int8_t push_state(StatePtr new_state, uint16_t arg) {
    if (state_stack_len < STATE_STACK_SIZE) {
        // TODO: call old state's exit hook?
        //       new hook for non-exit recursion into child?
        state_stack[state_stack_len] = new_state;
        state_stack_len ++;
        // FIXME: use EV_stacked_state?
        _set_state(new_state, arg, EV_leave_state, EV_enter_state);
        return state_stack_len;
    } else {
        // TODO: um...  how is a flashlight supposed to handle a recursion depth error?
        return -1;
    }
}

StatePtr pop_state() {
    // TODO: how to handle pop from empty stack?
    StatePtr old_state = NULL;
    StatePtr new_state = NULL;
    if (state_stack_len > 0) {
        state_stack_len --;
        old_state = state_stack[state_stack_len];
    }
    if (state_stack_len > 0) {
        new_state = state_stack[state_stack_len-1];
    }
    // FIXME: what should 'arg' be?
    _set_state(new_state, 0, EV_leave_state, EV_reenter_state);
    return old_state;
}

uint8_t set_state(StatePtr new_state, uint16_t arg) {
    // FIXME: this calls exit/enter hooks it shouldn't
    pop_state();
    return push_state(new_state, arg);
}

// bottom state on stack
// handles default actions for LVP, thermal regulation, etc
uint8_t default_state(EventPtr event, uint16_t arg) {
    if (0) {}

    #ifdef USE_LVP
    else if (event == EV_voltage_low) {
        low_voltage();
        return 0;
    }
    #endif

    #if 0
    #ifdef USE_THERMAL_REGULATION
    else if (event == EV_temperature_high) {
        high_temperature();
        return 0;
    }

    else if (event == EV_temperature_low) {
        low_temperature();
        return 0;
    }
    #endif
    #endif

    // event not handled
    return 1;
}

#endif
