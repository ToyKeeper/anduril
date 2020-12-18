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
    if (!num) { ontime = 8; num ++; }

    for (; num>0; num--) {
        set_level(BLINK_BRIGHTNESS);
        nice_delay_ms(ontime);
        set_level(0);
        nice_delay_ms(BLINK_SPEED * 3 / 12);
    }
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
        #ifdef AVRXMEGA3  // ATTINY816, 817, etc
        case 0:  // indicator off
            AUXLED_PORT.DIRSET = AUXLED_PIN; // set as output
            AUXLED_PORT.OUTCLR = AUXLED_PIN; // set output low
            #ifdef AUXLED2_PIN  // second LED mirrors the first
            AUXLED2_PORT.DIRSET = AUXLED2_PIN; // set as output
            AUXLED2_PORT.OUTCLR = AUXLED2_PIN; // set output low
            #endif
            break;
        case 1:  // indicator low
            AUXLED_PORT.DIRCLR = AUXLED_PIN; // set as input
            AUXLED_PORT.AUXLED_CTRL = PORT_PULLUPEN_bm; // enable internal pull-up
            #ifdef AUXLED2_PIN  // second LED mirrors the first
            AUXLED2_PORT.DIRCLR = AUXLE2D_PIN; // set as input
            AUXLED2_PORT.AUXLED2_CTRL = PORT_PULLUPEN_bm; // enable internal pull-up
            #endif
            break;
        default:  // indicator high
            AUXLED_PORT.DIRSET = AUXLED_PIN; // set as output
            AUXLED_PORT.OUTSET = AUXLED_PIN; // set as high
            #ifdef AUXLED2_PIN  // second LED mirrors the first
            AUXLED2_PORT.DIRSET = AUXLED2_PIN; // set as output
            AUXLED2_PORT.OUTSET = AUXLED2_PIN; // set as high
            #endif
            break;
      
        #else
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

        #endif
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

        #ifdef AVRXMEGA3  // ATTINY816, 817, etc
        case 0:  // LED off
            BUTTON_LED_PORT.DIRSET = BUTTON_LED_PIN; // set as output
            BUTTON_LED_PORT.OUTCLR = BUTTON_LED_PIN; // set output low
            break;
        case 1:  // LED low
            BUTTON_LED_PORT.DIRCLR = BUTTON_LED_PIN; // set as input
            BUTTON_LED_PORT.BUTTON_LED_CTRL = PORT_PULLUPEN_bm; // enable internal pull-up
            break;
        default:  // LED high
            BUTTON_LED_PORT.DIRSET = BUTTON_LED_PIN; // set as output
            BUTTON_LED_PORT.OUTSET = BUTTON_LED_PIN; // set as high
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

        #endif
    }
}
#endif

#ifdef USE_AUX_RGB_LEDS
#ifdef AVRXMEGA3  // ATTINY816, 817, etc
#error Function rgb_led_set in fsm-misc is currently not set up for the AVR 1-Series
#endif
void rgb_led_set(uint8_t value) {
    // value: 0b00BBGGRR
    uint8_t pins[] = { AUXLED_R_PIN, AUXLED_G_PIN, AUXLED_B_PIN };
    for (uint8_t i=0; i<3; i++) {
        uint8_t lvl = (value >> (i<<1)) & 0x03;
        uint8_t pin = pins[i];
        switch (lvl) {
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

#ifdef USE_REBOOT
void reboot() {
    // put the WDT in hard reset mode, then trigger it
    cli();
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85)
        WDTCR = 0xD8 | WDTO_15MS;
    #elif (ATTINY == 1634)
        // allow protected configuration changes for next 4 clock cycles
        CCP = 0xD8;  // magic number
        // reset (WDIF + WDE), no WDIE, fastest (16ms) timing (0000)
        // (DS section 8.5.2 and table 8-4)
        WDTCSR = 0b10001000;
    #elif defined(AVRXMEGA3)  // ATTINY816, 817, etc
        CCP = CCP_IOREG_gc;  // temporarily disable change protection
        WDT.CTRLA = WDT_PERIOD_8CLK_gc;  // Enable, timeout 8ms
    #endif
    sei();
    wdt_reset();
    while (1) {}
}
#endif

#endif
