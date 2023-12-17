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
#endif

/*
#ifdef USE_BLINK
uint8_t blink(uint8_t num, uint8_t speed);
#endif
*/

#ifdef USE_INDICATOR_LED
// FIXME: Remove this, replace with button_led()
// lvl: 0=off, 1=low, 2=high
void indicator_led(uint8_t lvl);
#endif

#ifdef USE_BUTTON_LED
// lvl: 0=off, 1=low, 2=high
void button_led_set(uint8_t lvl);
#endif

// if any type of aux LEDs exist, define a shorthand flag for it
#if defined(USE_INDICATOR_LED) || defined(USE_AUX_RGB_LEDS) || defined(USE_BUTTON_LED)
#define HAS_AUX_LEDS
#endif

#ifdef USE_AUX_RGB_LEDS
// value: 0b00BBGGRR
// each pair of bits: 0=off, 1=low, 2=high
void rgb_led_set(uint8_t value);
#endif

#ifdef USE_TRIANGLE_WAVE
uint8_t triangle_wave(uint8_t phase);
#endif

