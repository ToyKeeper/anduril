// Noctigon DM11 boost driver layout (attiny1634)
// Copyright (C) 2021-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * (based on Noctigon K1)
 *
 * Pin / Name / Function
 *   1    PA6   (none) (PWM1B) (reserved for DD drivers)
 *   2    PA5   R: red aux LED (PWM0B)
 *   3    PA4   G: green aux LED
 *   4    PA3   B: blue aux LED
 *   5    PA2   L: button LED
 *   6    PA1   (none)
 *   7    PA0   (none)
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   (none)
 *  11    PC4   (none)
 *  12    PC3   RESET
 *  13    PC2   (none)
 *  14    PC1   SCK
 *  15    PC0   boost PMIC enable (PWM0A not used)
 *  16    PB3   main LED PWM (PWM1A)
 *  17    PB2   MISO (PCINT10)
 *  18    PB1   MOSI / battery voltage (ADC6)
 *  19    PB0   Opamp power
 *  20    PA7   e-switch  (PCINT7)
 *      ADC12   thermal sensor
 *
 * Main LED power uses one pin to turn the Opamp on/off,
 * and one pin to control Opamp power level.
 * Regulated brightness control uses the power level pin, with PWM+DSM.
 * The on/off pin is only used to turn the main LED on and off,
 * not to change brightness.
 */

#define HWDEF_C  hank/noctigon-dm11/boost/hwdef.c

// allow using aux LEDs as extra channel modes
#include "fsm/chan-rgbaux.h"

// channel modes:
// * 0. main LEDs
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


#define PWM_CHANNELS  1  // old, remove this

#define PWM_BITS      16        // 0 to 32640 (0 to 255 PWM + 0 to 127 DSM) at constant kHz
#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // 15-bit PWM+DSM ramp

#define PWM_TOP       ICR1   // holds the TOP value for variable-resolution PWM
#define PWM_TOP_INIT  255
#define PWM_CNT       TCNT1  // for checking / resetting phase
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
#define DSM_TOP       (255<<7) // 15-bit resolution leaves 1 bit for carry

// timer interrupt for DSM
#define DSM_vect     TIMER1_OVF_vect
#define DSM_INTCTRL  TIMSK
#define DSM_OVF_bm   (1<<TOIE1)

#define DELAY_FACTOR 90  // less time in delay() because more time spent in interrupts

// regulated channel
uint16_t ch1_dsm_lvl;
uint8_t ch1_pwm, ch1_dsm;
#define CH1_PIN          PB3    // pin 16, Opamp reference
#define CH1_PWM          OCR1A  // OCR1A is the output compare register for PB3

#define CH1_ENABLE_PIN   PB0    // pin 19, Opamp power
#define CH1_ENABLE_PORT  PORTB  // control port for PB0

#define CH1_ENABLE_PIN2  PC0   // pin 15, boost PMIC enable
#define CH1_ENABLE_PORT2 PORTC // control port for PC0

// e-switch
#define SWITCH_PIN   PA7     // pin 20
#define SWITCH_PCINT PCINT7  // pin 20 pin change interrupt
#define SWITCH_PCIE  PCIE0   // PCIE0 is for PCINT[7:0]
#define SWITCH_PCMSK PCMSK0  // PCMSK0 is for PCINT[7:0]
#define SWITCH_PORT  PINA    // PINA or PINB or PINC
#define SWITCH_PUE   PUEA    // pullup group A
#define SWITCH_VECT  PCINT0_vect  // ISR for PCINT[7:0]

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

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// it also has an independent LED in the button
#define USE_BUTTON_LED
// the aux LEDs are front-facing, so turn them off while main LEDs are on
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif


inline void hwdef_setup() {
    // enable output ports
    // boost PMIC on/off
    DDRC = (1 << CH1_ENABLE_PIN2);
    // Opamp level and Opamp on/off
    DDRB = (1 << CH1_PIN)
         | (1 << CH1_ENABLE_PIN);
    // aux R/G/B, button LED
    DDRA = (1 << AUXLED_R_PIN)
         | (1 << AUXLED_G_PIN)
         | (1 << AUXLED_B_PIN)
         | (1 << BUTTON_LED_PIN)
         ;

    // configure PWM
    // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
    // pre-scale for timer: N = 1
    // PWM for both channels
    // WGM1[3:0]: 1,0,1,0: PWM, Phase Correct, adjustable (DS table 12-5)
    // WGM1[3:0]: 1,1,1,0: PWM, Fast, adjustable (DS table 12-5)
    // CS1[2:0]:    0,0,1: clk/1 (No prescaling) (DS table 12-6)
    // COM1A[1:0]:    1,0: PWM OC1A in the normal direction (DS table 12-4)
    // COM1B[1:0]:    1,0: PWM OC1B in the normal direction (DS table 12-4)
    TCCR1A = (1<<WGM11)  | (0<<WGM10)   // adjustable PWM (TOP=ICR1) (DS table 12-5)
           | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A in normal direction (DS table 12-4)
           | (1<<COM1B1) | (0<<COM1B0)  // PWM 1B in normal direction (DS table 12-4)
           ;
    TCCR1B = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
           //| (1<<WGM13)  | (1<<WGM12)  // fast adjustable PWM (DS table 12-5)
           | (1<<WGM13)  | (0<<WGM12)  // phase-correct adjustable PWM (DS table 12-5)
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

