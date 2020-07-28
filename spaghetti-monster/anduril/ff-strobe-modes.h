/*
 * ff-strobe-modes.h: Fireflies Flashlights strobe modes for Anduril.
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

#ifndef FF_STROBE_MODES_H
#define FF_STROBE_MODES_H

uint8_t boring_strobe_state(Event event, uint16_t arg);
inline void boring_strobe_state_iter();
uint8_t boring_strobe_type = 0;
void sos_blink(uint8_t num, uint8_t dah);
#ifdef USE_POLICE_STROBE_MODE
inline void police_strobe_iter();
#endif
#define NUM_BORING_STROBES 2


#endif
