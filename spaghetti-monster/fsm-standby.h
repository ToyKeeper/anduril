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

#ifdef TICK_DURING_STANDBY
#ifndef STANDBY_TICK_SPEED
#define STANDBY_TICK_SPEED 3  // every 0.128 s
/*
 * From the Attiny85 manual:
 * 0: 16 ms
 * 1: 32 ms
 * 2: 64 ms
 * 3: 0.128 s
 * 4: 0.256 s
 * 5: 0.512 s
 * 6: 1.0 s
 * 7: 2.0 s
 * 32: 4.0 s
 * 33: 8.0 s
 * (other values may have unexpected effects; not sure why the final bit is
 *  separated from the others, in the "32" position instead of "8", but that's
 *  how it is)
 */
#endif
#if (STANDBY_TICK_SPEED == 1)
#define SLEEP_TICKS_PER_SECOND 31
#elif (STANDBY_TICK_SPEED == 2)
#define SLEEP_TICKS_PER_SECOND 16
#elif (STANDBY_TICK_SPEED == 3)
#define SLEEP_TICKS_PER_SECOND 8
#elif (STANDBY_TICK_SPEED == 4)
#define SLEEP_TICKS_PER_SECOND 4
#elif (STANDBY_TICK_SPEED == 5)
#define SLEEP_TICKS_PER_SECOND 2
#elif (STANDBY_TICK_SPEED == 6)
#define SLEEP_TICKS_PER_SECOND 1
#endif
#endif

#define standby_mode sleep_until_eswitch_pressed
void sleep_until_eswitch_pressed();

#ifdef USE_IDLE_MODE
// stops processing until next click or timer tick
// (I think)
void idle_mode();
#endif

#endif
