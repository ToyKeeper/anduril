/*
 * ramp-mode-fsm.h: FSM config for ramping functions in Anduril.
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

#ifndef RAMP_MODE_FSM_H
#define RAMP_MODE_FSM_H

// enable FSM's ramping features
#define USE_RAMPING

// do smooth adjustments when compensating for temperature
#ifdef USE_THERMAL_REGULATION
#define USE_SET_LEVEL_GRADUALLY  // isn't used except for thermal adjustments
#endif

// brightness to use when no memory is set
// FIXME: this is only here to stop an error in fsm-ramping.c,
//        which should be fixed by using a different symbol instead
//        (like BUTTON_LED_BRIGHT_LEVEL or RAMP_HALFWAY_LEVEL or something)
#ifndef DEFAULT_LEVEL
#define DEFAULT_LEVEL MAX_1x7135
#endif


#endif
