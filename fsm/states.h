// fsm-states.h: State-handling functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "fsm/adc.h"

// typedefs
typedef uint8_t State(Event event, uint16_t arg);
typedef State * StatePtr;

// top of the stack
volatile StatePtr current_state;

// stack for states, to allow shared utility states like "input a number"
// and such, which return to the previous state after finishing
#define STATE_STACK_SIZE 8
StatePtr state_stack[STATE_STACK_SIZE];
uint8_t state_stack_len = 0;

void _set_state(StatePtr new_state, uint16_t arg,
                Event exit_event, Event enter_event);
int8_t push_state(StatePtr new_state, uint16_t arg);
StatePtr pop_state();
uint8_t set_state(StatePtr new_state, uint16_t arg);

// if loop() needs to change state, use this instead of set_state()
// (because this avoids race conditions)
volatile StatePtr deferred_state;
volatile uint16_t deferred_state_arg;
void set_state_deferred(StatePtr new_state, uint16_t arg);

#ifndef DONT_USE_DEFAULT_STATE
uint8_t default_state(Event event, uint16_t arg);
#endif

