// Emisar 2-channel-plus-FET-joined-into-1-channel
// Copyright (C) 2024 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * Pin / Name / Function
 *   1    PA6   ch2 LED PWM (linear) (PWM1B) (unused)
 *   2    PA5   R: red aux LED (PWM0B)
 *   3    PA4   G: green aux LED
 *   4    PA3   B: blue aux LED
 *   5    PA2   button LED
 *   6    PA1   Opamp 2 enable (channel 2 LEDs) (unused)
 *   7    PA0   Opamp 1 enable (channel 1 LEDs)
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   (none)
 *  11    PC4   (none)
 *  12    PC3   RESET
 *  13    PC2   (none)
 *  14    PC1   SCK
 *  15    PC0   ch1 LED PWM (FET) (PWM0A, 8-bit)
 *  16    PB3   ch1 LED PWM (linear) (PWM1A)
 *  17    PB2   MISO
 *  18    PB1   MOSI / battery voltage (ADC6)
 *  19    PB0   (none)
 *  20    PA7   e-switch (PCINT7)
 *      ADC12   thermal sensor
 *
 * This is used for lights with only 1 set of main LEDs, when using the
 * 2-channel driver internally.  This allows the D4S to have a lighted button.
 * Hank typically connects only linear ch1 and sometimes the DD FET
 * to the main LEDs, leaving linear ch2 completely unused.  Support is
 * included in an extra channel mode in case modders decide to use it.
 * It can raise the maximum regulated level.
 */

#define HWDEF_C  hank/emisar-2ch/fet/joined/hwdef.c

// allow using aux LEDs as extra channel modes
#include "fsm/chan-rgbaux.h"

// channel modes:
// * 0. linear (ch1) only
// * 1. linear (ch1) + DD FET  <-- Hank's primary config, as far as I can tell
// x 2. linear (both) + DD FET (not used)
#define NUM_CHANNEL_MODES   (2 + NUM_RGB_AUX_CHANNEL_MODES)
enum channel_modes_e {
    CM_CH1 = 0,
    CM_CH1_FET,    // <-- default
    //CM_BOTH_FET,
    RGB_AUX_ENUMS
};

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b0000000000000010
//#define USE_CHANNEL_MODE_ARGS
// _, _, _, 128=middle CCT, 0=warm-to-cool
//#define CHANNEL_MODE_ARGS     0,0,0,RGB_AUX_CM_ARGS

#define PWM_CHANNELS 3  // old, remove this

#define PWM_BITS      16        // 0 to 32640 (0 to 255 PWM + 0 to 127 DSM)
#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t
// linear-only ramp
#define PWM1_DATATYPE uint16_t
#define PWM1_GET(l)   PWM_GET16(pwm1_levels, l)
// linear part of linear+FET ramp
#define PWM2_DATATYPE uint16_t
#define PWM2_GET(l)   PWM_GET16(pwm2_levels, l)
// DD FET part of linear+FET ramp
#define PWM3_DATATYPE uint8_t
#define PWM3_GET(l)   PWM_GET8(pwm3_levels, l)

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP       ICR1   // holds the TOP value for for variable-resolution PWM
#define PWM_TOP_INIT  255
#define PWM_CNT       TCNT1  // for checking / resetting phase
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
#define DSM_TOP       (255<<7) // 15-bit resolution leaves 1 bit for carry

// timer interrupt for DSM
#define DSM_vect     TIMER1_OVF_vect
#define DSM_INTCTRL  TIMSK
#define DSM_OVF_bm   (1<<TOIE1)

#define DELAY_FACTOR 90  // less time in delay() because more time spent in interrupts

// main LEDs, linear
uint16_t ch1_dsm_lvl;
uint8_t ch1_pwm, ch1_dsm;
#define CH1_PIN  PB3            // pin 16, Opamp reference
#define CH1_PWM  OCR1A          // OCR1A is the output compare register for PB3
#define CH1_ENABLE_PIN   PA0    // pin 7, Opamp power
#define CH1_ENABLE_PORT  PORTA  // control port for PA0

// 2nd LEDs, linear (unused)
#define CH2_PIN  PA6            // pin 1, 2nd LED Opamp reference
#define CH2_PWM  OCR1B          // OCR1B is the output compare register for PA6
#define CH2_ENABLE_PIN   PA1    // pin 6, Opamp power
#define CH2_ENABLE_PORT  PORTA  // control port for PA1

// main LEDs, DD FET
#define CH3_PIN  PC0            // pin 15, DD FET PWM
#define CH3_PWM  OCR0A          // OCR0A is the output compare register for PC0

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
    DDRC = (1 << CH3_PIN);
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
    TCCR1A = (1<<WGM11)  | (0<<WGM10)   // adjustable PWM (TOP=ICR1) (DS table 12-5)
           | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A in normal direction (DS table 12-4)
           | (1<<COM1B1) | (0<<COM1B0)  // PWM 1B in normal direction (DS table 12-4)
           ;
    TCCR1B = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
           | (1<<WGM13)  | (0<<WGM12)  // phase-correct adjustable PWM (DS table 12-5)
           ;

    // FET PWM (8-bit; this channel can't do 10-bit)
    // WGM0[2:0]: 0,0,1: PWM, Phase Correct, 8-bit (DS table 11-8)
    // CS0[2:0]:  0,0,1: clk/1 (No prescaling) (DS table 11-9)
    // COM0A[1:0]:  1,0: PWM OC0A in the normal direction (DS table 11-4)
    // COM0B[1:0]:  1,0: PWM OC0B in the normal direction (DS table 11-7)
    TCCR0A = (0<<WGM01)  | (1<<WGM00)   // 8-bit (TOP=0xFF) (DS table 11-8)
           | (1<<COM0A1) | (0<<COM0A0)  // PWM 0A in normal direction (DS table 11-4)
           //| (1<<COM0B1) | (0<<COM0B0)  // PWM 0B in normal direction (DS table 11-7)
           ;
    TCCR0B = (0<<CS02)   | (0<<CS01) | (1<<CS00)  // clk/1 (no prescaling) (DS table 11-9)
           | (0<<WGM02)  // phase-correct PWM (DS table 11-8)
           ;

    // set PWM resolution
    PWM_TOP = PWM_TOP_INIT;

    // set up interrupt for delta-sigma modulation
    // (moved to hwdef.c functions so it can be enabled/disabled based on ramp level)
    //DSM_INTCTRL |= DSM_OVF_bm;  // interrupt once for each timer cycle

    // set up e-switch
    SWITCH_PUE = (1 << SWITCH_PIN);  // pull-up for e-switch
    SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}


#define LAYOUT_DEFINED

