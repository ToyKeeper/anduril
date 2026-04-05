// aux-leds.h: Aux LED functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#if defined(USE_INDICATOR_LED) && defined(TICK_DURING_STANDBY)
void indicator_led_update(uint8_t mode, uint8_t tick);
#endif
#if defined(USE_AUX_RGB_LEDS) && defined(TICK_DURING_STANDBY)
uint8_t setting_rgb_mode_now = 0;
void rgb_led_update(uint8_t mode, uint16_t arg);
void rgb_led_voltage_readout(uint8_t bright);
/*
 * 0: R
 * 1: RG
 * 2:  G
 * 3:  GB
 * 4:   B
 * 5: R B
 * 6: RGB
 * 7: rainbow
 * 8: voltage
 */
const PROGMEM uint8_t rgb_led_colors[] = {
    0b00000000,  // 0: black
    0b00000001,  // 1: red
    0b00000101,  // 2: yellow
    0b00000100,  // 3: green
    0b00010100,  // 4: cyan
    0b00010000,  // 5: blue
    0b00010001,  // 6: purple
    0b00010101,  // 7: white
};
// RGB aux LED pattern numbers (upper nibble of mode byte)
#define RGB_PATTERN_OFF       0
#define RGB_PATTERN_LOW       1
#define RGB_PATTERN_HIGH      2
#define RGB_PATTERN_BLINKING  3
#ifdef USE_RGB_ANIMATION_MODES
#define RGB_PATTERN_HEARTBEAT 4
#define RGB_PATTERN_BREATHING 5
#define RGB_PATTERN_PULSE     6
#endif
#define RGB_PATTERN_FIRST     RGB_PATTERN_OFF
#ifdef USE_RGB_ANIMATION_MODES
#define RGB_PATTERN_LAST      RGB_PATTERN_PULSE
#else
#define RGB_PATTERN_LAST      RGB_PATTERN_BLINKING
#endif

// RGB aux LED color numbers (lower nibble of mode byte)
#define RGB_COLOR_RED      0
#define RGB_COLOR_YELLOW   1
#define RGB_COLOR_GREEN    2
#define RGB_COLOR_CYAN     3
#define RGB_COLOR_BLUE     4
#define RGB_COLOR_PURPLE   5
#define RGB_COLOR_WHITE    6
#define RGB_COLOR_DISCO    7
#define RGB_COLOR_RAINBOW  8
#define RGB_COLOR_VOLTAGE  9

// intentionally 1 higher than total modes, to make "voltage" easier to reach
// (at Hank's request)
#define RGB_LED_NUM_COLORS 11
#define RGB_LED_NUM_PATTERNS (RGB_PATTERN_LAST - RGB_PATTERN_FIRST + 1)
#ifndef RGB_LED_OFF_DEFAULT
#define RGB_LED_OFF_DEFAULT  ((RGB_PATTERN_LOW      << 4) | RGB_COLOR_VOLTAGE)  // 0x19
//#define RGB_LED_OFF_DEFAULT  ((RGB_PATTERN_LOW      << 4) | RGB_COLOR_RAINBOW)  // 0x18
#endif
#ifndef RGB_LED_LOCKOUT_DEFAULT
#define RGB_LED_LOCKOUT_DEFAULT  ((RGB_PATTERN_BLINKING << 4) | RGB_COLOR_VOLTAGE)  // 0x39
//#define RGB_LED_LOCKOUT_DEFAULT  ((RGB_PATTERN_BLINKING << 4) | RGB_COLOR_DISCO)    // 0x37
#endif
#ifndef RGB_RAINBOW_SPEED
#define RGB_RAINBOW_SPEED 0x0f  // change color every 16 frames
#endif
#endif

// Indicator LED pattern numbers (4-bit nibble, packed into cfg.indicator_led_mode)
// upper nibble = lockout mode, lower nibble = off mode
#define INDICATOR_PATTERN_OFF       0
#define INDICATOR_PATTERN_LOW       1
#define INDICATOR_PATTERN_HIGH      2
#define INDICATOR_PATTERN_BLINKING  3  // requires TICK_DURING_STANDBY
#ifdef USE_INDICATOR_ANIMATION_MODES
#define INDICATOR_PATTERN_HEARTBEAT 4  // requires TICK_DURING_STANDBY
#define INDICATOR_PATTERN_BREATHING 5  // requires TICK_DURING_STANDBY
#define INDICATOR_PATTERN_PULSE     6  // requires TICK_DURING_STANDBY
#endif
#define INDICATOR_PATTERN_FIRST     INDICATOR_PATTERN_OFF
#ifdef USE_INDICATOR_ANIMATION_MODES
#define INDICATOR_PATTERN_LAST      INDICATOR_PATTERN_PULSE
#else
#define INDICATOR_PATTERN_LAST      INDICATOR_PATTERN_BLINKING
#endif

//#define USE_OLD_BLINKING_INDICATOR
//#define USE_FANCIER_BLINKING_INDICATOR
#ifdef USE_INDICATOR_LED
    // upper nibble (bits 4-7) controls lockout mode
    // lower nibble (bits 0-3) controls "off" mode
    // modes are: 0=off, 1=low, 2=high, 3=blinking, 4=heartbeat, 5=breathing
    // (modes 3-5 require TICK_DURING_STANDBY)
    #ifndef INDICATOR_LED_DEFAULT_MODE
        #ifdef USE_INDICATOR_LED_WHILE_RAMPING
            #define INDICATOR_LED_DEFAULT_MODE ((INDICATOR_PATTERN_HIGH     << 4) | INDICATOR_PATTERN_LOW)
        #else
            #define INDICATOR_LED_DEFAULT_MODE ((INDICATOR_PATTERN_BLINKING << 4) | INDICATOR_PATTERN_LOW)
        #endif
    #endif
#endif

