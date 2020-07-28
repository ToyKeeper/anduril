/*
 * candle-mode.h: Candle mode for Anduril.
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

#ifndef CANDLE_MODE_H
#define CANDLE_MODE_H

#ifndef CANDLE_AMPLITUDE
#define CANDLE_AMPLITUDE 25
#endif

uint8_t candle_mode_state(Event event, uint16_t arg);
// moved to fsm-misc.c because it's also used for tint ramping power correction
//uint8_t triangle_wave(uint8_t phase);


#endif
