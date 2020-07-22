/*
 * lockout.h: Lockout mode for Anduril.
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

#ifndef LOCKOUT_H
#define LOCKOUT_H

// should lockout mode function as a momentary moon mode?
#define MOON_DURING_LOCKOUT_MODE
// if enabled, 2nd lockout click goes to the other ramp's floor level
#define LOCKOUT_MOON_FANCY
// soft lockout
uint8_t lockout_state(Event event, uint16_t arg);


#endif
