// fsm-misc.h: Miscellaneous function for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef USE_DYNAMIC_UNDERCLOCKING
void auto_clock_speed();
#endif

// shortest time (in ms) the light should blink for to indicate a zero
#ifndef BLINK_ONCE_TIME
    #define BLINK_ONCE_TIME 10
#endif

#if defined(USE_BLINK_NUM) || defined(USE_BLINK_DIGIT)
    #ifndef BLINK_BRIGHTNESS
        #define BLINK_BRIGHTNESS (MAX_LEVEL/6)
    #endif
    #if defined(USE_CFG) && defined(DEFAULT_BLINK_CHANNEL)
        #define BLINK_CHANNEL cfg.blink_channel
    #elif defined(DEFAULT_BLINK_CHANNEL)
        #define BLINK_CHANNEL DEFAULT_BLINK_CHANNEL
    #endif
    uint8_t blink_digit(uint8_t num);
#endif

#ifdef USE_BLINK_NUM
//#define USE_BLINK
uint8_t blink_num(uint8_t num);
#ifdef USE_LONG_BLINK_FOR_NEGATIVE_SIGN
void blink_negative();
#endif
#endif

/*
#ifdef USE_BLINK
uint8_t blink(uint8_t num, uint8_t speed);
#endif
*/

#ifdef USE_TRIANGLE_WAVE
uint8_t triangle_wave(uint8_t phase);
#endif

