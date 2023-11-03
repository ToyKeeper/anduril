// fsm-states.c: State-handling functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "fsm/states.h"
#include "fsm/adc.h"

// TODO: if callback doesn't handle current event,
//       pass event to next state on stack?
//       Callback return values:
//       0: event handled normally
//       1: event not handled
//       255: error (not sure what this would even mean though, or what difference it would make)
// TODO: function to call stacked callbacks until one returns "handled"

void _set_state(StatePtr new_state, uint16_t arg,
                Event exit_event, Event enter_event) {
    // call old state-exit hook (don't use stack)
    if (current_state != NULL) current_state(exit_event, arg);
    // set new state
    current_state = new_state;
    // call new state-enter hook (don't use stack)
    if (new_state != NULL) current_state(enter_event, arg);

    // since state changed, stop any animation in progress
    interrupt_nice_delays();
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
    // FIXME: what should 'arg' be?  (maybe re-entry should be entry with arg+1?)
    _set_state(new_state, 0, EV_leave_state, EV_reenter_state);
    return old_state;
}

uint8_t set_state(StatePtr new_state, uint16_t arg) {
    // FIXME: this calls exit/enter hooks it shouldn't
    //        (for the layer underneath the top)
    pop_state();
    return push_state(new_state, arg);
}

void set_state_deferred(StatePtr new_state, uint16_t arg) {
    deferred_state = new_state;
    deferred_state_arg = arg;
}

#ifndef DONT_USE_DEFAULT_STATE
// bottom state on stack
// handles default actions for LVP, thermal regulation, etc
uint8_t default_state(Event event, uint16_t arg) {
    if (0) {}  // this should get compiled out

    #ifdef USE_LVP
    else if (event == EV_voltage_low) {
        low_voltage();
        return EVENT_HANDLED;
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
    return EVENT_NOT_HANDLED;
}
#endif

