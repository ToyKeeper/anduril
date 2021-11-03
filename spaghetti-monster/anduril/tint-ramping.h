/*
 * tint-ramping.h: Tint ramping functions for Anduril.
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

#ifndef TINT_RAMPING_H
#define TINT_RAMPING_H

// 0: smooth tint ramp
// 1: toggle tint only between two extremes
#ifdef TINT_RAMP_TOGGLE_ONLY
uint8_t tint_style = 1;
#else
uint8_t tint_style = 0;
#endif

#ifdef USE_MANUAL_MEMORY
uint8_t manual_memory_tint;
#endif

// not actually a mode, more of a fallback under other modes
uint8_t tint_ramping_state(Event event, uint16_t arg);


#endif
