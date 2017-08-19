#ifndef TK_DELAY_H
#define TK_DELAY_H
/*
 * Smaller, more flexible replacement(s) for default _delay_ms() functions.
 *
 * Copyright (C) 2015 Selene ToyKeeper
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
 *
 */

#ifdef OWN_DELAY
#include "tk-attiny.h"
#include <util/delay_basic.h>
#ifdef USE_DELAY_MS
// Having own _delay_ms() saves some bytes AND adds possibility to use variables as input
#define delay_ms _delay_ms
void _delay_ms(uint16_t n)
{
    // TODO: make this take tenths of a ms instead of ms,
    // for more precise timing?
    //#ifdef USE_FINE_DELAY
    //if (n==0) { _delay_loop_2(BOGOMIPS/3); }
    //else {
    //    while(n-- > 0) _delay_loop_2(BOGOMIPS);
    //}
    //#else
    while(n-- > 0) _delay_loop_2(BOGOMIPS);
    //#endif
}
#endif
#if defined(USE_FINE_DELAY) || defined(USE_DELAY_ZERO)
#define delay_zero _delay_zero
void _delay_zero() {
    _delay_loop_2(BOGOMIPS/3);
}
#endif
#ifdef USE_DELAY_4MS
#define delay_4ms _delay_4ms
void _delay_4ms(uint8_t n)  // because it saves a bit of ROM space to do it this way
{
    while(n-- > 0) _delay_loop_2(BOGOMIPS*4);
}
#endif
#ifdef USE_DELAY_S
#define delay_s _delay_s
void _delay_s()  // because it saves a bit of ROM space to do it this way
{
  #ifdef USE_DELAY_4MS
    _delay_4ms(250);
  #else
    #ifdef USE_DELAY_MS
    _delay_ms(1000);
    #endif
  #endif
}
#endif
#else
#include <util/delay.h>
#endif


#endif  // TK_DELAY_H
