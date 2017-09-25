/*
 * fsm-standby.h: standby mode functions for SpaghettiMonster.
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

#ifndef FSM_STANDBY_H
#define FSM_STANDBY_H

// deferred "off" so we won't suspend in a weird state
// (like...  during the middle of a strobe pulse)
// set this to nonzero to enter standby mode next time the system is idle
volatile uint8_t go_to_standby = 0;

#define standby_mode sleep_until_eswitch_pressed
void sleep_until_eswitch_pressed();

#ifdef USE_IDLE_MODE
// deferred "idle" state trigger
// stops processing until next click or timer tick
//volatile uint8_t go_to_idle = 0;
void idle_mode();
#endif

// TODO: half-sleep "twilight" mode with WDT on but running slowly

#endif
