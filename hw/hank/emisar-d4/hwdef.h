// Emisar D4 driver layout
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 *           ----
 *   Reset -|1  8|- VCC
 * eswitch -|2  7|-
 * AUX LED -|3  6|- PWM (FET)
 *     GND -|4  5|- PWM (1x7135)
 *           ----
 */

#define HWDEF_C  hank/emisar-d4/hwdef.c

// allow using aux LEDs as extra channel modes (when they exist)
//#ifdef AUXLED_PIN
#if 0
#include "fsm/chan-aux.h"
#else
#define NUM_AUX_CHANNEL_MODES  0
#endif

// channel modes
// * 0. FET+7135 stacked
// * 1. button LED (only on some derivative models, like BLF Q8)
#define NUM_CHANNEL_MODES   (1 + NUM_AUX_CHANNEL_MODES)
enum CHANNEL_MODES {
    CM_MAIN = 0,
    CM_AUX,
};

#define DEFAULT_CHANNEL_MODE  CM_MAIN

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b00000001


#define PWM_CHANNELS 2  // old, remove this

#define PWM_BITS      8       // attiny85 only supports up to 8 bits
#define PWM_GET       PWM_GET8
#define PWM_DATATYPE  uint8_t
#define PWM_DATATYPE2 uint16_t
#define PWM1_DATATYPE uint8_t   // 1x7135 ramp
#define PWM2_DATATYPE uint8_t   // DD FET ramp

#define PWM_TOP_INIT  255    // highest value used in top half of ramp

// 1x7135 channel
#ifndef CH1_PIN
#define CH1_PIN  PB0        // pin 5, 1x7135 PWM
#define CH1_PWM  OCR0A      // OCR0A is the output compare register for PB0
#endif

// DD FET channel
#ifndef CH2_PIN
#define CH2_PIN  PB1        // pin 6, FET PWM
#define CH2_PWM  OCR0B      // OCR0B is the output compare register for PB1
#endif

//#define AUXLED_PIN   PB4    // pin 3

// e-switch
#ifndef SWITCH_PIN
#define SWITCH_PIN   PB3    // pin 2
#define SWITCH_PCINT PCINT3 // pin 2 pin change interrupt
#endif

// (FIXME: remove?  not used?)
//#define VOLTAGE_PIN PB2     // pin 7, voltage ADC
//#define ADC_CHANNEL 0x01    // MUX 01 corresponds with PB2
//#define ADC_DIDR    ADC1D   // Digital input disable bit corresponding with PB2
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
         | (1 << CH2_PIN);

    TCCR0B = 0x01; // pre-scaler for timer (1 => 1, 2 => 8, 3 => 64...)
    TCCR0A = PHASE;

    // configure e-switch
    PORTB = (1 << SWITCH_PIN);  // e-switch is the only input
    PCMSK = (1 << SWITCH_PIN);  // pin change interrupt uses this pin
}


#define LAYOUT_DEFINED

