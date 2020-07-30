/*
 * sunset-timer.h: Sunset / candle auto-shutoff functions for Anduril.
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

#ifndef SUNSET_TIMER_H
#define SUNSET_TIMER_H

// how many minutes to add each time the user "bumps" the timer?
#define SUNSET_TIMER_UNIT 10

#define TICKS_PER_MINUTE (TICKS_PER_SECOND*60)

// automatic shutoff timer
uint8_t sunset_timer = 0;
uint8_t sunset_timer_peak = 0;
uint16_t sunset_ticks = 0;
uint8_t sunset_timer_state(Event event, uint16_t arg);


#endif
