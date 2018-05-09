/*
 * fsm-pcint.h: PCINT (Pin Change Interrupt) functions for SpaghettiMonster.
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

#ifndef FSM_PCINT_H
#define FSM_PCINT_H

//static volatile uint8_t button_was_pressed;
#define BP_SAMPLES 32
volatile uint8_t button_last_state;
uint8_t button_is_pressed();
inline void PCINT_on();
inline void PCINT_off();
void PCINT_inner(uint8_t pressed);

#endif
