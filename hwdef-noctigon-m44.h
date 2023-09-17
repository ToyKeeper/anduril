// hwdef for Noctigon M44 2-channel light
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * Pin / Name / Function
 *   1    PA6   ch2 LED PWM (boost) (PWM1B)
 *   2    PA5   R: red aux LED (PWM0B)
 *   3    PA4   G: green aux LED
 *   4    PA3   B: blue aux LED
 *   5    PA2   button LED
 *   6    PA1   (none)
 *   7    PA0   Opamp 2 enable (channel 2 LEDs)
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   (none)
 *  11    PC4   (none)
 *  12    PC3   RESET
 *  13    PC2   (none)
 *  14    PC1   SCK
 *  15    PC0   (none)
 *  16    PB3   ch1 LED PWM (linear) (PWM1A)
 *  17    PB2   MISO
 *  18    PB1   MOSI / battery voltage (ADC6)
 *  19    PB0   Opamp 1 enable (channel 1 LEDs)
 *  20    PA7   e-switch (PCINT7)
 *      ADC12   thermal sensor
 */

#define ATTINY 1634
#include <avr/io.h>

#define HWDEF_C_FILE hwdef-noctigon-m44.c

// allow using aux LEDs as extra channel modes
#include "chan-rgbaux.h"

// channel modes:
// * 0. channel 1 only
// * 1. channel 2 only
// * 2. both channels, tied together
// * 3. both channels, manual blend
// * 4? both channels, manual blend, max 200% power
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


#define PWM_CHANNELS 1  // old, remove this

#define PWM_BITS 16     // 0 to 16383 at variable Hz, not 0 to 255 at 16 kHz
#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // regular ramp table
//#define PWM2_DATATYPE uint16_t  // max "200% power" ramp table
//#define PWM3_DATATYPE uint16_t  // PWM_TOPs for 2nd ramp table

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP       ICR1   // holds the TOP value for for variable-resolution PWM
#define PWM_TOP_INIT  511
#define PWM_CNT       TCNT1  // for dynamic PWM, reset phase

#define CH1_PIN  PB3            // pin 16, Opamp reference
#define CH1_PWM  OCR1A          // OCR1A is the output compare register for PB3
#define CH1_ENABLE_PIN   PB0    // pin 19, Opamp power
#define CH1_ENABLE_PORT  PORTB  // control port for PB0

#define CH2_PIN  PA6            // pin 1, 2nd LED Opamp reference
#define CH2_PWM  OCR1B          // OCR1B is the output compare register for PA6
#define CH2_ENABLE_PIN   PA0    // pin 7, Opamp power
#define CH2_ENABLE_PORT  PORTA  // control port for PA0


// e-switch
#ifndef SWITCH_PIN
#define SWITCH_PIN   PA7     // pin 20
#define SWITCH_PCINT PCINT7  // pin 20 pin change interrupt
#define SWITCH_PCIE  PCIE0   // PCIE1 is for PCINT[7:0]
#define SWITCH_PCMSK PCMSK0  // PCMSK1 is for PCINT[7:0]
#define SWITCH_PORT  PINA    // PINA or PINB or PINC
#define SWITCH_PUE   PUEA    // pullup group A
#define PCINT_vect   PCINT0_vect  // ISR for PCINT[7:0]
#endif

#define USE_VOLTAGE_DIVIDER  // use a dedicated pin, not VCC, because VCC input is flattened
#define VOLTAGE_PIN PB1      // Pin 18 / PB1 / ADC6
// pin to ADC mappings are in DS table 19-4
#define VOLTAGE_ADC      ADC6D  // digital input disable pin for PB1
// DIDR0/DIDR1 mappings are in DS section 19.13.5, 19.13.6
#define VOLTAGE_ADC_DIDR DIDR1  // DIDR channel for ADC6D
// DS tables 19-3, 19-4
// Bit   7     6     5      4     3    2    1    0
//     REFS1 REFS0 REFEN ADC0EN MUX3 MUX2 MUX1 MUX0
// MUX[3:0] = 0, 1, 1, 0 for ADC6 / PB1
// divided by ...
// REFS[1:0] = 1, 0 for internal 1.1V reference
// other bits reserved
#define ADMUX_VOLTAGE_DIVIDER 0b10000110
#define ADC_PRSCL   0x07    // clk/128

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
//   (voltage - D1) * (R2/(R2+R1) * 1024 / 1.1)
// D1, R1, R2 = 0, 330, 100
#ifndef ADC_44
//#define ADC_44 981  // raw value at 4.40V
#define ADC_44 967  // manually tweaked so 4.16V will blink out 4.2
#endif
#ifndef ADC_22
//#define ADC_22 489  // raw value at 2.20V
#define ADC_22 482  // manually tweaked so 4.16V will blink out 4.2
#endif

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
    DDRB = (1 << CH1_PIN)
         | (1 << CH1_ENABLE_PIN)
         ;
    DDRA = (1 << CH2_PIN)
         | (1 << CH2_ENABLE_PIN)
         | (1 << AUXLED_R_PIN)
         | (1 << AUXLED_G_PIN)
         | (1 << AUXLED_B_PIN)
         | (1 << BUTTON_LED_PIN)
         ;

    // configure PWM
    // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
    // pre-scale for timer: N = 1
    // Linear opamp PWM for both main and 2nd LEDs (10-bit)
    // WGM1[3:0]: 1,0,1,0: PWM, Phase Correct, adjustable (DS table 12-5)
    // WGM1[3:0]: 1,1,1,0: PWM, Fast, adjustable (DS table 12-5)
    // CS1[2:0]:    0,0,1: clk/1 (No prescaling) (DS table 12-6)
    // COM1A[1:0]:    1,0: PWM OC1A in the normal direction (DS table 12-4)
    // COM1B[1:0]:    1,0: PWM OC1B in the normal direction (DS table 12-4)
    TCCR1A  = (1<<WGM11)  | (0<<WGM10)   // adjustable PWM (TOP=ICR1) (DS table 12-5)
            | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A in normal direction (DS table 12-4)
            | (1<<COM1B1) | (0<<COM1B0)  // PWM 1B in normal direction (DS table 12-4)
            ;
    TCCR1B  = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
            | (1<<WGM13)  | (1<<WGM12)  // fast adjustable PWM (DS table 12-5)
            //| (1<<WGM13)  | (0<<WGM12)  // phase-correct adjustable PWM (DS table 12-5)
            ;

    // set PWM resolution
    PWM_TOP = PWM_TOP_INIT;

    // set up e-switch
    SWITCH_PUE = (1 << SWITCH_PIN);  // pull-up for e-switch
    SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}

#define LAYOUT_DEFINED

