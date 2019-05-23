/*
 * fsm-misc.h: Miscellaneous function for SpaghettiMonster.
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

#ifndef FSM_MISC_H
#define FSM_MISC_H

#ifdef USE_DYNAMIC_UNDERCLOCKING
void auto_clock_speed();
#endif

#if defined(USE_BLINK_NUM) || defined(USE_BLINK_DIGIT)
#ifndef BLINK_BRIGHTNESS
#define BLINK_BRIGHTNESS (MAX_LEVEL/6)
#endif
uint8_t blink_digit(uint8_t num);
#endif

#ifdef USE_BLINK_NUM
//#define USE_BLINK
uint8_t blink_num(uint8_t num);
#endif

/*
#ifdef USE_BLINK
uint8_t blink(uint8_t num, uint8_t speed);
#endif
*/

#ifdef USE_INDICATOR_LED
void indicator_led(uint8_t lvl);
#endif

#ifdef USE_TRIANGLE_WAVE
uint8_t triangle_wave(uint8_t phase);
#endif

#endif
