/*
 * fsm-states.h: State-handling functions for SpaghettiMonster.
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
 
#ifndef FSM_STATES_H
#define FSM_STATES_H

#include "fsm-adc.h"

// typedefs
typedef uint8_t State(EventPtr event, uint16_t arg);
typedef State * StatePtr;

// top of the stack
volatile StatePtr current_state;

// stack for states, to allow shared utility states like "input a number"
// and such, which return to the previous state after finishing
#define STATE_STACK_SIZE 8
StatePtr state_stack[STATE_STACK_SIZE];
uint8_t state_stack_len = 0;

void _set_state(StatePtr new_state, uint16_t arg,
                EventPtr exit_event, EventPtr enter_event);
int8_t push_state(StatePtr new_state, uint16_t arg);
StatePtr pop_state();
uint8_t set_state(StatePtr new_state, uint16_t arg);
uint8_t default_state(EventPtr event, uint16_t arg);

#endif
