// fsm-misc.c: Miscellaneous function for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
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
#endif

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
#endif

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
#endif
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
#endif

#ifdef USE_INDICATOR_LED
void indicator_led(uint8_t lvl) {
    switch (lvl) {
        // FIXME: move this logic to arch/*
        #if (MCU==0x1616) || (MCU==0x32dd20)  // ATTINY816, 817, etc

        case 0:  // indicator off
            AUXLED_PORT.DIRSET = (1 << AUXLED_PIN); // set as output
            AUXLED_PORT.OUTCLR = (1 << AUXLED_PIN); // set output low
            #ifdef AUXLED2_PIN  // second LED mirrors the first
            AUXLED2_PORT.DIRSET = (1 << AUXLED2_PIN); // set as output
            AUXLED2_PORT.OUTCLR = (1 << AUXLED2_PIN); // set output low
            #endif
            break;
        case 1:  // indicator low
            AUXLED_PORT.DIRCLR = (1 << AUXLED_PIN); // set as input
            // this resolves to PORTx.PINxCTRL = PORT_PULLUPEN_bm;
            *((uint8_t *)&AUXLED_PORT + 0x10 + AUXLED_PIN) = PORT_PULLUPEN_bm; // enable internal pull-up
            #ifdef AUXLED2_PIN  // second LED mirrors the first
            AUXLED2_PORT.DIRCLR = (1 << AUXLED2_PIN); // set as input
            // this resolves to PORTx.PINxCTRL = PORT_PULLUPEN_bm;
            *((uint8_t *)&AUXLED2_PORT + 0x10 + AUXLED2_PIN) = PORT_PULLUPEN_bm; // enable internal pull-up
            #endif
            break;
        default:  // indicator high
            AUXLED_PORT.DIRSET = (1 << AUXLED_PIN); // set as output
            AUXLED_PORT.OUTSET = (1 << AUXLED_PIN); // set as high
            #ifdef AUXLED2_PIN  // second LED mirrors the first
            AUXLED2_PORT.DIRSET = (1 << AUXLED2_PIN); // set as output
            AUXLED2_PORT.OUTSET = (1 << AUXLED2_PIN); // set as high
            #endif
            break;

        #else  // MCU is old tiny style, not newer mega style

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

        #endif  // MCU type
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

#ifdef USE_BUTTON_LED
// TODO: Refactor this and RGB LED function to merge code and save space
void button_led_set(uint8_t lvl) {
    switch (lvl) {

        // FIXME: move this logic to arch/*
        #if (MCU==0x1616) || (MCU==0x32dd20)  // ATTINY816, 817, etc

        case 0:  // LED off
            BUTTON_LED_PORT.DIRSET = (1 << BUTTON_LED_PIN); // set as output
            BUTTON_LED_PORT.OUTCLR = (1 << BUTTON_LED_PIN); // set output low
            break;
        case 1:  // LED low
            BUTTON_LED_PORT.DIRCLR = (1 << BUTTON_LED_PIN); // set as input
            // this resolves to PORTx.PINxCTRL = PORT_PULLUPEN_bm;
            *((uint8_t *)&BUTTON_LED_PORT + 0x10 + BUTTON_LED_PIN) = PORT_PULLUPEN_bm; // enable internal pull-up
            break;
        default:  // LED high
            BUTTON_LED_PORT.DIRSET = (1 << BUTTON_LED_PIN); // set as output
            BUTTON_LED_PORT.OUTSET = (1 << BUTTON_LED_PIN); // set as high
            break;

        #else

        case 0:  // LED off
            BUTTON_LED_DDR  &= 0xff ^ (1 << BUTTON_LED_PIN);
            BUTTON_LED_PUE  &= 0xff ^ (1 << BUTTON_LED_PIN);
            BUTTON_LED_PORT &= 0xff ^ (1 << BUTTON_LED_PIN);
            break;
        case 1:  // LED low
            BUTTON_LED_DDR  &= 0xff ^ (1 << BUTTON_LED_PIN);
            BUTTON_LED_PUE  |= (1 << BUTTON_LED_PIN);
            BUTTON_LED_PORT |= (1 << BUTTON_LED_PIN);
            break;
        default:  // LED high
            BUTTON_LED_DDR  |= (1 << BUTTON_LED_PIN);
            BUTTON_LED_PUE  |= (1 << BUTTON_LED_PIN);
            BUTTON_LED_PORT |= (1 << BUTTON_LED_PIN);
            break;

        #endif  // MCU type
    }
}
#endif

#ifdef USE_AUX_RGB_LEDS
void rgb_led_set(uint8_t value) {
    // value: 0b00BBGGRR
    uint8_t pins[] = { AUXLED_R_PIN, AUXLED_G_PIN, AUXLED_B_PIN };
    for (uint8_t i=0; i<3; i++) {
        uint8_t lvl = (value >> (i<<1)) & 0x03;
        uint8_t pin = pins[i];
        switch (lvl) {

            // FIXME: move this logic to arch/*
            #if (MCU==0x1616) || (MCU==0x32dd20)  // ATTINY816, 817, etc

            case 0:  // LED off
                AUXLED_RGB_PORT.DIRSET = (1 << pin); // set as output
                AUXLED_RGB_PORT.OUTCLR = (1 << pin); // set output low
                break;
            case 1:  // LED low
                AUXLED_RGB_PORT.DIRCLR = (1 << pin); // set as input
                // this resolves to PORTx.PINxCTRL = PORT_PULLUPEN_bm;
                *((uint8_t *)&AUXLED_RGB_PORT + 0x10 + pin) = PORT_PULLUPEN_bm; // enable internal pull-up
                break;
            default:  // LED high
                AUXLED_RGB_PORT.DIRSET = (1 << pin); // set as output
                AUXLED_RGB_PORT.OUTSET = (1 << pin); // set as high
                break;

            #else

            case 0:  // LED off
                AUXLED_RGB_DDR  &= 0xff ^ (1 << pin);
                AUXLED_RGB_PUE  &= 0xff ^ (1 << pin);
                AUXLED_RGB_PORT &= 0xff ^ (1 << pin);
                break;
            case 1:  // LED low
                AUXLED_RGB_DDR  &= 0xff ^ (1 << pin);
                AUXLED_RGB_PUE  |= (1 << pin);
                AUXLED_RGB_PORT |= (1 << pin);
                break;
            default:  // LED high
                AUXLED_RGB_DDR  |= (1 << pin);
                AUXLED_RGB_PUE  |= (1 << pin);
                AUXLED_RGB_PORT |= (1 << pin);
                break;

            #endif  // MCU type
        }
    }
}
#endif  // ifdef USE_AUX_RGB_LEDS

#ifdef USE_TRIANGLE_WAVE
uint8_t triangle_wave(uint8_t phase) {
    uint8_t result = phase << 1;
    if (phase > 127) result = 255 - result;
    return result;
}
#endif

