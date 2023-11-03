// delay.h: Smaller, more flexible _delay_ms() functions.
// Copyright (C) 2015-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#ifdef OWN_DELAY
#include "arch/mcu.h"
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
    //_delay_loop_2((BOGOMIPS/3) & 0xff00);
    _delay_loop_2(DELAY_ZERO_TIME);
}
#endif
#ifdef USE_DELAY_4MS
#ifndef delay_4ms
#define delay_4ms _delay_4ms
void _delay_4ms(uint8_t n)  // because it saves a bit of ROM space to do it this way
{
    while(n-- > 0) _delay_loop_2(BOGOMIPS*4);
}
#endif
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

