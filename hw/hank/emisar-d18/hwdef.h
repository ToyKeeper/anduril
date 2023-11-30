// Emisar D18 (FET+13+1) driver layout
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 *           ----
 *   Reset -|1  8|- VCC
 * eswitch -|2  7|- aux LED?
 *     FET -|3  6|- 13x7135
 *     GND -|4  5|-  1x7135
 *           ----
 */

#define HWDEF_C  lumintop/fw3a/hwdef.c

// channel modes
// * 0. FET+N+1 stacked
#define NUM_CHANNEL_MODES   1
enum CHANNEL_MODES {
    CM_MAIN = 0,
};

#define DEFAULT_CHANNEL_MODE  CM_MAIN

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b00000001


#define PWM_CHANNELS 3  // old, remove this

#define PWM_BITS      8       // attiny85 only supports up to 8 bits
#define PWM_GET       PWM_GET8
#define PWM_DATATYPE  uint8_t
#define PWM_DATATYPE2 uint16_t
#define PWM1_DATATYPE uint8_t   // 1x7135 ramp
#define PWM2_DATATYPE uint8_t   // 7x7135 ramp
#define PWM3_DATATYPE uint8_t   // DD FET ramp

#define PWM_TOP_INIT  255    // highest value used in top half of ramp

// 1x7135 channel
#define CH1_PIN  PB0        // pin 5, 1x7135 PWM
#define CH1_PWM  OCR0A      // OCR0A is the output compare register for PB0

// 7x7135 channel
#define CH2_PIN  PB1        // pin 6, 7x7135 PWM
#define CH2_PWM  OCR0B      // OCR0B is the output compare register for PB1

// DD FET channel
#define CH3_PIN  PB4        // pin 3, FET PWM
#define CH3_PWM  OCR1B      // OCR1B is the output compare register for PB4

// e-switch
#ifndef SWITCH_PIN
#define SWITCH_PIN   PB3    // pin 2
#define SWITCH_PCINT PCINT3 // pin 2 pin change interrupt
#endif

#ifndef AUXLED_PIN
#define AUXLED_PIN PB2      // pin 7
#endif
#define ADC_PRSCL   0x07    // clk/128

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 5  // add 0.25V
#endif

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels


inline void hwdef_setup() {

    // configure PWM channels
    DDRB = (1 << CH1_PIN)
         | (1 << CH2_PIN)
         | (1 << CH3_PIN);

    // configure PWM channels
    TCCR0B = 0x01; // pre-scaler for timer (1 => 1, 2 => 8, 3 => 64...)
    TCCR0A = PHASE;

    // Second PWM counter is ... weird
    TCCR1 = _BV (CS10);
    GTCCR = _BV (COM1B1) | _BV (PWM1B);
    OCR1C = PWM_TOP_INIT;  // Set ceiling value to maximum

    // configure e-switch
    PORTB = (1 << SWITCH_PIN);  // e-switch is the only input
    PCMSK = (1 << SWITCH_PIN);  // pin change interrupt uses this pin

}


#define LAYOUT_DEFINED

