// fsm-misc.c: Miscellaneous function for SpaghettiMonster.
// Copyright (C) 2017-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef USE_DYNAMIC_UNDERCLOCKING
void auto_clock_speed() {
    uint8_t level = actual_level;  // volatile, avoid repeat access
    if (level < QUARTERSPEED_LEVEL) {
        // run at quarter speed
        // note: this only works when executed as two consecutive instructions
        // (don't try to combine them or put other stuff between)
        clock_prescale_set(clock_div_4);
    }
    else if (level < HALFSPEED_LEVEL) {
        // run at half speed
        clock_prescale_set(clock_div_2);
    } else {
        // run at full speed
        clock_prescale_set(clock_div_1);
    }
}
#endif  // #ifdef USE_DYNAMIC_UNDERCLOCKING

#if defined(USE_BLINK_NUM) || defined(USE_BLINK_DIGIT)
#define BLINK_SPEED 1000
uint8_t blink_digit(uint8_t num) {
    //StatePtr old_state = current_state;

    // "zero" digit gets a single short blink
    uint8_t ontime = BLINK_SPEED * 2 / 12;
    if (!num) { ontime = BLINK_ONCE_TIME; num ++; }

    #ifdef BLINK_CHANNEL
    // channel is set per blink, to prevent issues
    // if another mode interrupts us (like a config menu)
    uint8_t old_channel = channel_mode;
    #endif

    for (; num>0; num--) {
        // TODO: allow setting a blink channel mode per build target
        #ifdef BLINK_CHANNEL
            set_channel_mode(BLINK_CHANNEL);
        #endif
        set_level(BLINK_BRIGHTNESS);
        #ifdef BLINK_CHANNEL
            channel_mode = old_channel;
        #endif
        nice_delay_ms(ontime);

        #ifdef BLINK_CHANNEL
            set_channel_mode(BLINK_CHANNEL);
        #endif
        set_level(0);
        #ifdef BLINK_CHANNEL
            channel_mode = old_channel;
        #endif
        nice_delay_ms(BLINK_SPEED * 3 / 12);
    }

    #ifdef BLINK_CHANNEL
    set_channel_mode(old_channel);
    #endif

    return nice_delay_ms(BLINK_SPEED * 8 / 12);
}
#ifdef USE_LONG_BLINK_FOR_NEGATIVE_SIGN
void blink_negative() {
    // "negative" symbol gets a single long blink
    uint8_t ontime = BLINK_SPEED * 2 / 12;
    set_level(BLINK_BRIGHTNESS);
    nice_delay_ms(ontime * 3);
    set_level(0);
    nice_delay_ms(ontime * 5);
}
#endif  // #ifdef USE_LONG_BLINK_FOR_NEGATIVE_SIGN
#endif  // #if defined(USE_BLINK_NUM) || defined(USE_BLINK_DIGIT)

#ifdef USE_BLINK_BIG_NUM
uint8_t blink_big_num(uint16_t num) {
    uint16_t digits[] = { 10000, 1000, 100, 10, 1 };
    uint8_t started = 0;
    for (uint8_t digit=0; digit<sizeof(digits)/sizeof(uint16_t); digit++) {
        uint16_t scale = digits[digit];
        if (num >= scale) {
            started = 1;
        }
        if (started) {
            uint8_t digit = 0;
            while (num >= scale) {
                num -= scale;
                digit ++;
            }
            if (! blink_digit(digit)) return 0;
        }
    }

    return nice_delay_ms(1000);
}
#endif  // #ifdef USE_BLINK_BIG_NUM

#ifdef USE_BLINK_NUM
uint8_t blink_num(uint8_t num) {
    #if 1
    uint8_t hundreds = num / 100;
    num = num % 100;
    uint8_t tens = num / 10;
    num = num % 10;
    #else  // can be smaller or larger, depending on whether divmod is used elsewhere
    uint8_t hundreds = 0;
    uint8_t tens = 0;
    for(; num >= 100; hundreds ++, num -= 100);
    for(; num >= 10; tens ++, num -= 10);
    #endif

    #if 0
    // wait a moment in the dark before starting
    set_level(0);
    nice_delay_ms(200);
    #endif

    if (hundreds) blink_digit(hundreds);
    if (hundreds || tens) blink_digit(tens);
    return blink_digit(num);
}
#endif  // #ifdef USE_BLINK_NUM

#ifdef USE_TRIANGLE_WAVE
uint8_t triangle_wave(uint8_t phase) {
    uint8_t result = phase << 1;
    if (phase > 127) result = 255 - result;
    return result;
}
#endif  // #ifdef USE_TRIANGLE_WAVE

