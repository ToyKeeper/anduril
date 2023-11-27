// Emisar 2-channel generic w/ tint ramping
// Copyright (C) 2021-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * Pin / Name / Function
 *   1    PA6   ch2 LED PWM (linear) (PWM1B)
 *   2    PA5   R: red aux LED (PWM0B)
 *   3    PA4   G: green aux LED
 *   4    PA3   B: blue aux LED
 *   5    PA2   button LED
 *   6    PA1   Opamp 2 enable (channel 2 LEDs)
 *   7    PA0   Opamp 1 enable (channel 1 LEDs)
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   (none)
 *  11    PC4   (none)
 *  12    PC3   RESET
 *  13    PC2   (none)
 *  14    PC1   SCK
 *  15    PC0   [unused: ch1 LED PWM (FET) (PWM0A, 8-bit)]
 *  16    PB3   ch1 LED PWM (linear) (PWM1A)
 *  17    PB2   MISO
 *  18    PB1   MOSI / battery voltage (ADC6)
 *  19    PB0   (none)
 *  20    PA7   e-switch (PCINT7)
 *      ADC12   thermal sensor
 *
 * Both sets of LEDs use one pin to turn the Opamp on/off,
 * and one pin to control the Opamp power level.
 */

#define HWDEF_C  hank/emisar-2ch/hwdef.c

// allow using aux LEDs as extra channel modes
#include "fsm/chan-rgbaux.h"

// channel modes:
// * 0. channel 1 only
// * 1. channel 2 only
// * 2. both channels, tied together, max "200%" power
// * 3. both channels, manual blend, max "100%" power
// * 4. both channels, auto blend, reversible
#define NUM_CHANNEL_MODES   (5 + NUM_RGB_AUX_CHANNEL_MODES)
enum channel_modes_e {
    CM_CH1 = 0,
    CM_CH2,
    CM_BOTH,
    CM_BLEND,
    CM_AUTO,
    RGB_AUX_ENUMS
};

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b0000000000011111
#define USE_CHANNEL_MODE_ARGS
// _, _, _, 128=middle CCT, 0=warm-to-cool
#define CHANNEL_MODE_ARGS     0,0,0,128,0,RGB_AUX_CM_ARGS

// can use some of the common handlers
#define USE_CALC_2CH_BLEND


#define PWM_CHANNELS 2  // old, remove this

#define PWM_BITS      16     // 0 to 16383 at variable Hz, not 0 to 255 at 16 kHz
#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // regular ramp table
#define PWM2_DATATYPE uint16_t  // max "200% power" ramp table
//#define PWM3_DATATYPE uint8_t   // DD FET ramp table (8-bit only)

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP       ICR1   // holds the TOP value for for variable-resolution PWM
#define PWM_TOP_INIT  511    // highest value used in top half of ramp
#define PWM_CNT       TCNT1  // for dynamic PWM, reset phase

// main LEDs, linear
#define CH1_PIN  PB3            // pin 16, Opamp reference
#define CH1_PWM  OCR1A          // OCR1A is the output compare register for PB3
#define CH1_ENABLE_PIN   PA0    // pin 7, Opamp power
#define CH1_ENABLE_PORT  PORTA  // control port for PA0

// 2nd LEDs, linear
#define CH2_PIN  PA6            // pin 1, 2nd LED Opamp reference
#define CH2_PWM  OCR1B          // OCR1B is the output compare register for PA6
#define CH2_ENABLE_PIN   PA1    // pin 6, Opamp power
#define CH2_ENABLE_PORT  PORTA  // control port for PA1

// main LEDs, DD FET
//#define CH3_PIN  PC0            // pin 15, DD FET PWM
//#define CH3_PWM  OCR0A          // OCR0A is the output compare register for PC0

// e-switch
#ifndef SWITCH_PIN
#define SWITCH_PIN   PA7     // pin 20
#define SWITCH_PCINT PCINT7  // pin 20 pin change interrupt
#define SWITCH_PCIE  PCIE0   // PCIE1 is for PCINT[7:0]
#define SWITCH_PCMSK PCMSK0  // PCMSK1 is for PCINT[7:0]
#define SWITCH_PORT  PINA    // PINA or PINB or PINC
#define SWITCH_PUE   PUEA    // pullup group A
#define SWITCH_VECT  PCINT0_vect  // ISR for PCINT[7:0]
#endif

#include "hank/vdivider-1634.h"

// this light has aux LEDs under the optic
#define AUXLED_R_PIN    PA5    // pin 2
#define AUXLED_G_PIN    PA4    // pin 3
#define AUXLED_B_PIN    PA3    // pin 4
#define AUXLED_RGB_PORT PORTA  // PORTA or PORTB or PORTC
#define AUXLED_RGB_DDR  DDRA   // DDRA or DDRB or DDRC
#define AUXLED_RGB_PUE  PUEA   // PUEA or PUEB or PUEC

#define BUTTON_LED_PIN  PA2    // pin 5
#define BUTTON_LED_PORT PORTA  // for all "PA" pins
#define BUTTON_LED_DDR  DDRA   // for all "PA" pins
#define BUTTON_LED_PUE  PUEA   // for all "PA" pins


inline void hwdef_setup() {
    // enable output ports
    //DDRC = (1 << CH3_PIN);
    DDRB = (1 << CH1_PIN);
    DDRA = (1 << CH2_PIN)
         | (1 << AUXLED_R_PIN)
         | (1 << AUXLED_G_PIN)
         | (1 << AUXLED_B_PIN)
         | (1 << BUTTON_LED_PIN)
         | (1 << CH1_ENABLE_PIN)
         | (1 << CH2_ENABLE_PIN)
         ;

    // configure PWM
    // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
    // pre-scale for timer: N = 1
    // Linear opamp PWM for both main and 2nd LEDs (10-bit)
    // WGM1[3:0]: 1,0,1,0: PWM, Phase Correct, adjustable (DS table 12-5)
    // CS1[2:0]:    0,0,1: clk/1 (No prescaling) (DS table 12-6)
    // COM1A[1:0]:    1,0: PWM OC1A in the normal direction (DS table 12-4)
    // COM1B[1:0]:    1,0: PWM OC1B in the normal direction (DS table 12-4)
    TCCR1A  = (1<<WGM11)  | (0<<WGM10)   // adjustable PWM (TOP=ICR1) (DS table 12-5)
            | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A in normal direction (DS table 12-4)
            | (1<<COM1B1) | (0<<COM1B0)  // PWM 1B in normal direction (DS table 12-4)
            ;
    TCCR1B  = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
            | (1<<WGM13)  | (0<<WGM12)  // phase-correct adjustable PWM (DS table 12-5)
            ;

    // unused on this driver
    #if 0
    // FET PWM (8-bit; this channel can't do 10-bit)
    // WGM0[2:0]: 0,0,1: PWM, Phase Correct, 8-bit (DS table 11-8)
    // CS0[2:0]:  0,0,1: clk/1 (No prescaling) (DS table 11-9)
    // COM0A[1:0]:  1,0: PWM OC0A in the normal direction (DS table 11-4)
    // COM0B[1:0]:  1,0: PWM OC0B in the normal direction (DS table 11-7)
    TCCR0A  = (0<<WGM01)  | (1<<WGM00)   // 8-bit (TOP=0xFF) (DS table 11-8)
            | (1<<COM0A1) | (0<<COM0A0)  // PWM 0A in normal direction (DS table 11-4)
            //| (1<<COM0B1) | (0<<COM0B0)  // PWM 0B in normal direction (DS table 11-7)
            ;
    TCCR0B  = (0<<CS02)   | (0<<CS01) | (1<<CS00)  // clk/1 (no prescaling) (DS table 11-9)
            | (0<<WGM02)  // phase-correct PWM (DS table 11-8)
            ;
    CH3_PWM = 0;  // ensure this channel is off, if it exists
    #endif

    // set PWM resolution
    PWM_TOP = PWM_TOP_INIT;

    // set up e-switch
    SWITCH_PUE = (1 << SWITCH_PIN);  // pull-up for e-switch
    SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}


#define LAYOUT_DEFINED

