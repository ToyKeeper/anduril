// Noctigon K1 driver layout (attiny1634)
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * (originally known as Emisar D1S V2)
 *
 * Pin / Name / Function
 *   1    PA6   (none) (PWM1B) (reserved for DD drivers)
 *   2    PA5   R: red aux LED (PWM0B)
 *   3    PA4   G: green aux LED
 *   4    PA3   B: blue aux LED
 *   5    PA2   (none) (reserved for L: button LED (on some models))
 *   6    PA1   (none)
 *   7    PA0   (none)
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   (none)
 *  11    PC4   (none)
 *  12    PC3   RESET
 *  13    PC2   (none)
 *  14    PC1   SCK
 *  15    PC0   (none) PWM0A
 *  16    PB3   main LED PWM (PWM1A)
 *  17    PB2   MISO
 *  18    PB1   MOSI / battery voltage (ADC6)
 *  19    PB0   Opamp power
 *  20    PA7   e-switch  (PCINT7)
 *      ADC12   thermal sensor
 *
 * Main LED power uses one pin to turn the Opamp on/off,
 * and one pin to control Opamp power level.
 * All brightness control uses the power level pin, with 4 kHz 10-bit PWM.
 * The on/off pin is only used to turn the main LED on and off,
 * not to change brightness.
 */

#ifndef HWDEF_C
#define HWDEF_C  hank/noctigon-k1/hwdef.c
#endif

// allow using aux LEDs as extra channel modes
#include "fsm/chan-rgbaux.h"

// channel modes:
// * 0. main LED
// * 1+. aux RGB
#define NUM_CHANNEL_MODES   (1 + NUM_RGB_AUX_CHANNEL_MODES)
enum CHANNEL_MODES {
    CM_MAIN = 0,
    RGB_AUX_ENUMS
};

#define DEFAULT_CHANNEL_MODE  CM_MAIN

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b0000000000000001
// no args
//#define USE_CHANNEL_MODE_ARGS
//#define CHANNEL_MODE_ARGS     0,0,0,0,0,0,0,0


#define PWM_CHANNELS 1  // old, remove this

#define PWM_BITS      10        // 0 to 1023 at 4 kHz, not 0 to 255 at 16 kHz
#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t  // is used for PWM_TOPS (which goes way over 255)
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // linear ramp

#define PWM_TOP       ICR1   // holds the TOP value for variable-resolution PWM
#define PWM_TOP_INIT  1023   // highest value used in top half of ramp
#define PWM_CNT       TCNT1  // for dynamic PWM, reset phase

// linear channel
#define CH1_PIN  PB3            // pin 16, Opamp reference
#define CH1_PWM  OCR1A          // OCR1A is the output compare register for PB3
#define CH1_ENABLE_PIN   PB0    // pin 19, Opamp power
#define CH1_ENABLE_PORT  PORTB  // control port for PB0

// e-switch
#define SWITCH_PIN   PA7    // pin 20
#define SWITCH_PCINT PCINT7 // pin 20 pin change interrupt
#define SWITCH_PCIE  PCIE0  // PCIE0 is for PCINT[7:0]
#define SWITCH_PCMSK PCMSK0 // PCMSK0 is for PCINT[7:0]
#define SWITCH_PORT  PINA   // PINA or PINB or PINC
#define SWITCH_PUE   PUEA   // pullup group A
#define SWITCH_VECT  PCINT0_vect  // ISR for PCINT[7:0]


#include "hank/vdivider-1634.h"

// this light has aux LEDs under the optic
#define AUXLED_R_PIN    PA5    // pin 2
#define AUXLED_G_PIN    PA4    // pin 3
#define AUXLED_B_PIN    PA3    // pin 4
#define AUXLED_RGB_PORT PORTA  // PORTA or PORTB or PORTC
#define AUXLED_RGB_DDR  DDRA   // DDRA or DDRB or DDRC
#define AUXLED_RGB_PUE  PUEA   // PUEA or PUEB or PUEC


inline void hwdef_setup() {
    // enable output ports
    // Opamp level and Opamp on/off
    DDRB = (1 << CH1_PIN)
         | (1 << CH1_ENABLE_PIN);
    // aux R/G/B
    DDRA = (1 << AUXLED_R_PIN)
         | (1 << AUXLED_G_PIN)
         | (1 << AUXLED_B_PIN)
         ;

    // configure PWM
    // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
    // pre-scale for timer: N = 1
    // WGM1[3:0]: 0,0,1,1: PWM, Phase Correct, 10-bit (DS table 12-5)
    // CS1[2:0]:    0,0,1: clk/1 (No prescaling) (DS table 12-6)
    // COM1A[1:0]:    1,0: PWM OC1A in the normal direction (DS table 12-4)
    // COM1B[1:0]:    0,0: PWM OC1B disabled (DS table 12-4)
    TCCR1A  = (1<<WGM11)  | (1<<WGM10)   // 10-bit (TOP=0x03FF) (DS table 12-5)
            | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A in normal direction (DS table 12-4)
            | (0<<COM1B1) | (0<<COM1B0)  // PWM 1B in normal direction (DS table 12-4)
            ;
    TCCR1B  = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
            | (0<<WGM13)  | (0<<WGM12)  // phase-correct PWM (DS table 12-5)
            ;

    // set PWM resolution
    //PWM_TOP = PWM_TOP_INIT;

    // set up e-switch
    SWITCH_PUE = (1 << SWITCH_PIN);  // pull-up for e-switch
    SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}


#define LAYOUT_DEFINED

