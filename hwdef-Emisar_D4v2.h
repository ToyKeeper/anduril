#pragma once

/* hwdef for Emisar D4v2 (attiny1634)
 * Copyright (C) 2019 Selene ToyKeeper
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Pin / Name / Function
 *   1    PA6   FET PWM (PWM1B)
 *   2    PA5   red aux LED (PWM0B)
 *   3    PA4   green aux LED
 *   4    PA3   blue aux LED
 *   5    PA2   e-switch
 *   6    PA1   button LED
 *   7    PA0   (none)
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   (none)
 *  11    PC4   (none)
 *  12    PC3   RESET
 *  13    PC2   (none)
 *  14    PC1   SCK
 *  15    PC0   (none) PWM0A
 *  16    PB3   7135 PWM (PWM1A)
 *  17    PB2   MISO
 *  18    PB1   MOSI
 *  19    PB0   (none)
 *  20    PA7   (none)
 *      ADC12   thermal sensor
 */

#ifdef ATTINY
#undef ATTINY
#endif
#define ATTINY 1634
#include <avr/io.h>

// TODO: add aux red and aux blue as disabled channel modes,
// so they can be used for the police strobe?
#define NUM_CHANNEL_MODES 1
#define DEFAULT_CHANNEL_MODE 0
#define SET_LEVEL_MODES      set_level_2ch_stacked
#define GRADUAL_TICK_MODES   gradual_tick_2ch_stacked
// no special handlers needed, can use common handlers
#define USE_SET_LEVEL_2CH_STACKED
#define USE_GRADUAL_TICK_2CH_STACKED

#define PWM_CHANNELS 2

#define SWITCH_PIN   PA2    // pin 5
#define SWITCH_PCINT PCINT2 // pin 5 pin change interrupt
#define SWITCH_PCIE  PCIE0  // PCIE0 is for PCINT[7:0]
#define SWITCH_PCMSK PCMSK0 // PCMSK0 is for PCINT[7:0]
#define SWITCH_PORT  PINA   // PINA or PINB or PINC

#define LOW_PWM_PIN PB3     // pin 16, 1x7135 PWM
#define LOW_PWM_LVL OCR1A   // OCR1A is the output compare register for PB3
#define PWM1_BITS 8

#define HIGH_PWM_PIN PA6    // pin 1, FET PWM
#define HIGH_PWM_LVL OCR1B  // OCR1B is the output compare register for PB1
#define PWM2_BITS 8

// only using 8-bit on this light
#define PWM_GET       PWM_GET8
#define PWM_DATATYPE  uint8_t

#define ADC_PRSCL   0x07    // clk/128

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 4  // add 0.20V  (measured 0.22V)
#endif

// this light has aux LEDs under the optic
#define AUXLED_R_PIN    PA5    // pin 2
#define AUXLED_G_PIN    PA4    // pin 3
#define AUXLED_B_PIN    PA3    // pin 4
#define AUXLED_RGB_PORT PORTA  // PORTA or PORTB or PORTC
#define AUXLED_RGB_DDR  DDRA   // DDRA or DDRB or DDRC
#define AUXLED_RGB_PUE  PUEA   // PUEA or PUEB or PUEC

#define BUTTON_LED_PIN  PA1    // pin 6
#define BUTTON_LED_PORT PORTA  // for all "PA" pins
#define BUTTON_LED_DDR  DDRA   // for all "PA" pins
#define BUTTON_LED_PUE  PUEA   // for all "PA" pins

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// it also has an independent LED in the button
#define USE_BUTTON_LED
// the aux LEDs are front-facing, so turn them off while main LEDs are on
// TODO: the whole "indicator LED" thing needs to be refactored into
//       "aux LED(s)" and "button LED(s)" since they work a bit differently
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

inline void hwdef_setup() {
    // enable output ports
    // 7135
    DDRB = (1 << LOW_PWM_PIN);
    // FET, aux R/G/B, button LED
    DDRA = (1 << HIGH_PWM_PIN)
         | (1 << AUXLED_R_PIN)
         | (1 << AUXLED_G_PIN)
         | (1 << AUXLED_B_PIN)
         | (1 << BUTTON_LED_PIN)
         ;

    // configure PWM
    // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
    // pre-scale for timer: N = 1
    TCCR1A  = (0<<WGM11)  | (1<<WGM10)   // 8-bit (TOP=0xFF) (DS table 12-5)
            | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A in normal direction (DS table 12-4)
            | (1<<COM1B1) | (0<<COM1B0)  // PWM 1B in normal direction (DS table 12-4)
            ;
    TCCR1B  = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
            | (0<<WGM13)  | (0<<WGM12)  // phase-correct PWM (DS table 12-5)
            ;

    // set up e-switch
    //PORTA = (1 << SWITCH_PIN);  // TODO: configure PORTA / PORTB / PORTC?
    PUEA = (1 << SWITCH_PIN);  // pull-up for e-switch
    SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}


#define LAYOUT_DEFINED

