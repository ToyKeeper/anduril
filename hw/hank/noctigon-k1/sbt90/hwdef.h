// Noctigon K1-SBT90.2 driver layout (attiny1634)
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * (mostly the same as KR4 driver)
 *
 * Pin / Name / Function
 *   1    PA6   FET PWM (direct drive) (PWM1B)
 *   2    PA5   R: red aux LED (PWM0B)
 *   3    PA4   G: green aux LED
 *   4    PA3   B: blue aux LED
 *   5    PA2   (none)
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
 *  16    PB3   main LED PWM (linear) (PWM1A)
 *  17    PB2   MISO / e-switch (PCINT10)
 *  18    PB1   MOSI / battery voltage (ADC6)
 *  19    PB0   Opamp power
 *  20    PA7   (none)
 *      ADC12   thermal sensor
 *
 * Main LED power uses one pin to turn the Opamp on/off,
 * and one pin to control Opamp power level.
 * Main brightness control uses the power level pin, with dynamic PWM.
 * The on/off pin is only used to turn the main LED on and off,
 * not to change brightness.
 * Also has a direct-drive FET for turbo.
 */

#define HWDEF_C  hank/noctigon-kr4/hwdef.c

// allow using aux LEDs as extra channel modes
#include "fsm/chan-rgbaux.h"

// channel modes:
// * 0. linear + DD FET stacked
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


#define PWM_CHANNELS 2  // old, remove this

#define PWM_BITS      16        // dynamic 16-bit, but never goes over 255
#define PWM_GET       PWM_GET8
#define PWM_DATATYPE  uint16_t  // is used for PWM_TOPS (which goes way over 255)
#define PWM_DATATYPE2 uint16_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint8_t   // linear ramp
#define PWM2_DATATYPE uint8_t   // DD FET ramp

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP       ICR1   // holds the TOP value for variable-resolution PWM
#define PWM_TOP_INIT  255    // highest value used in top half of ramp
#define PWM_CNT       TCNT1  // for dynamic PWM, reset phase

// linear channel
#define CH1_PIN  PB3            // pin 16, Opamp reference
#define CH1_PWM  OCR1A          // OCR1A is the output compare register for PB3
#define CH1_ENABLE_PIN   PB0    // pin 19, Opamp power
#define CH1_ENABLE_PORT  PORTB  // control port for PB0

// DD FET channel
#define CH2_PIN  PA6            // pin 1, DD FET PWM
#define CH2_PWM  OCR1B          // OCR1B is the output compare register for PA6

// e-switch
#define SWITCH_PIN   PB2     // pin 17
#define SWITCH_PCINT PCINT10 // pin 17 pin change interrupt
#define SWITCH_PCIE  PCIE1   // PCIE1 is for PCINT[11:8]
#define SWITCH_PCMSK PCMSK1  // PCMSK1 is for PCINT[11:8]
#define SWITCH_PORT  PINB    // PINA or PINB or PINC
#define SWITCH_PUE   PUEB    // pullup group B
#define SWITCH_VECT  PCINT1_vect  // ISR for PCINT[11:8]

#include "hank/vdivider-1634.h"

#define TEMP_CHANNEL 0b00001111

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
    // DD FET PWM, aux R/G/B
    DDRA = (1 << CH2_PIN)
         | (1 << AUXLED_R_PIN)
         | (1 << AUXLED_G_PIN)
         | (1 << AUXLED_B_PIN)
         ;

    // configure PWM
    // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
    // pre-scale for timer: N = 1
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

    // set PWM resolution
    PWM_TOP = PWM_TOP_INIT;

    // set up e-switch
    SWITCH_PUE = (1 << SWITCH_PIN);  // pull-up for e-switch
    SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}


#define LAYOUT_DEFINED

