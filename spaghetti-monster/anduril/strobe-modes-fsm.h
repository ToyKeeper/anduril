/*
 * strobe-modes-fsm.h: FSM config for strobe modes in Anduril.
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

#ifndef STROBE_MODES_FSM_H
#define STROBE_MODES_FSM_H

// enable the random number generator if we need it
#if defined(USE_LIGHTNING_MODE) || defined(USE_CANDLE_MODE)
#define USE_PSEUDO_RAND
#endif

// party strobe uses really short pulses
#ifdef USE_PARTY_STROBE_MODE
#define USE_DELAY_ZERO
#endif

// candle mode is basically a bunch of stacked random triangle waves
#if defined(USE_CANDLE_MODE)
#define USE_TRIANGLE_WAVE
#endif

// the presence of strobe mode(s) affects how many eeprom bytes we need,
// so it's relevant for FSM configuration
#if defined(USE_CANDLE_MODE) || defined(USE_BIKE_FLASHER_MODE) || defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE) || defined(USE_LIGHTNING_MODE)
#define USE_STROBE_STATE
#endif

#endif
