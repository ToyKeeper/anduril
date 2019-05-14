/*
 * fsm-misc.c: Miscellaneous function for SpaghettiMonster.
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

#ifndef FSM_MISC_C
#define FSM_MISC_C

#ifdef USE_DYNAMIC_UNDERCLOCKING
void auto_clock_speed() {
    uint8_t level = actual_level;  // volatile, avoid repeat access
    if (level < QUARTERSPEED_LEVEL) {
        // run at quarter speed
        // note: this only works when executed as two consecutive instructions
        // (don't try to combine them or put other stuff between)
        CLKPR = 1<<CLKPCE; CLKPR = 2;
    }
    else if (level < HALFSPEED_LEVEL) {
        // run at half speed
        CLKPR = 1<<CLKPCE; CLKPR = 1;
    } else {
        // run at full speed
        CLKPR = 1<<CLKPCE; CLKPR = 0;
    }
}
#endif

#if defined(USE_BLINK_NUM) || defined(USE_BLINK_DIGIT)
uint8_t blink_digit(uint8_t num) {
    //StatePtr old_state = current_state;

    // "zero" digit gets a single short blink
    uint8_t ontime = 200;
    if (!num) { ontime = 8; num ++; }

    for (; num>0; num--) {
        set_level(BLINK_BRIGHTNESS);
        nice_delay_ms(ontime);
        set_level(0);
        nice_delay_ms(400);
    }
    return nice_delay_ms(600);
}
#endif

#ifdef USE_BLINK_NUM
uint8_t blink_num(uint8_t num) {
    //StatePtr old_state = current_state;
    #if 0
    uint8_t hundreds = num / 100;
    num = num % 100;
    uint8_t tens = num / 10;
    num = num % 10;
    #else  // 8 bytes smaller
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

    #if 0
    if (hundreds) {
        if (! blink_digit(hundreds)) return 0;
        if (! blink_digit(tens)) return 0;
    }
    else if (tens) {
        if (! blink_digit(tens)) return 0;
    }
    if (! blink_digit(num)) return 0;
    return nice_delay_ms(1000);
    #else // same size :(
    if (hundreds) if (! blink_digit(hundreds)) return 0;
    if (hundreds || tens) if (! blink_digit(tens)) return 0;
    if (! blink_digit(num)) return 0;
    return nice_delay_ms(1000);
    #endif

    /*
    uint8_t volts, tenths;
    volts = voltage / 10;
    tenths = voltage % 10;
    if (! blink(volts)) return;
    nice_delay_ms(200);
    if (! blink(tenths)) return;
    nice_delay_ms(200);
    */
}
#endif

#ifdef USE_INDICATOR_LED
void indicator_led(uint8_t lvl) {
    switch (lvl) {
        case 0:  // indicator off
            DDRB &= 0xff ^ (1 << AUXLED_PIN);
            PORTB &= 0xff ^ (1 << AUXLED_PIN);
            #ifdef AUXLED2_PIN  // second LED mirrors the first
            DDRB &= 0xff ^ (1 << AUXLED2_PIN);
            PORTB &= 0xff ^ (1 << AUXLED2_PIN);
            #endif
            break;
        case 1:  // indicator low
            DDRB &= 0xff ^ (1 << AUXLED_PIN);
            PORTB |= (1 << AUXLED_PIN);
            #ifdef AUXLED2_PIN  // second LED mirrors the first
            DDRB &= 0xff ^ (1 << AUXLED2_PIN);
            PORTB |= (1 << AUXLED2_PIN);
            #endif
            break;
        default:  // indicator high
            DDRB |= (1 << AUXLED_PIN);
            PORTB |= (1 << AUXLED_PIN);
            #ifdef AUXLED2_PIN  // second LED mirrors the first
            DDRB |= (1 << AUXLED2_PIN);
            PORTB |= (1 << AUXLED2_PIN);
            #endif
            break;
    }
}

/*
void indicator_led_auto() {
    if (actual_level > MAX_1x7135) indicator_led(2);
    else if (actual_level > 0) indicator_led(1);
    else indicator_led(0);
}
*/
#endif  // USE_INDICATOR_LED

#ifdef USE_TRIANGLE_WAVE
uint8_t triangle_wave(uint8_t phase) {
    uint8_t result = phase << 1;
    if (phase > 127) result = 255 - result;
    return result;
}
#endif

#endif
