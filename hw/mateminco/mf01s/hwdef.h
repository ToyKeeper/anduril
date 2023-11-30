// Mateminco MT18S / Astrolux MF01S driver layout
// Copyright (C) 2019-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 *           ----
 *   Reset -|1  8|- VCC (unused)
 * eswitch -|2  7|- Voltage divider (2S)
 * AUX LED -|3  6|- PWM (FET)
 *     GND -|4  5|- PWM (smaller FET)
 *           ----
 */

#define HWDEF_C  hank/emisar-d4/hwdef.c

// channel modes
// * 0. small FET + big FET stacked
#define NUM_CHANNEL_MODES  1
enum CHANNEL_MODES {
    CM_MAIN = 0,
};

#define DEFAULT_CHANNEL_MODE  CM_MAIN

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b00000001


#define PWM_CHANNELS 2  // old, remove this

#define PWM_BITS      8       // attiny85 only supports up to 8 bits
#define PWM_GET       PWM_GET8
#define PWM_DATATYPE  uint8_t
#define PWM_DATATYPE2 uint16_t
#define PWM1_DATATYPE uint8_t   // little FET ramp
#define PWM2_DATATYPE uint8_t   // big FET ramp

#define PWM_TOP_INIT  255    // highest value used in top half of ramp

// little FET channel
#define CH1_PIN  PB0        // pin 5, 1x7135 PWM
#define CH1_PWM  OCR0A      // OCR0A is the output compare register for PB0

// big FET channel
#define CH2_PIN  PB1        // pin 6, FET PWM
#define CH2_PWM  OCR0B      // OCR0B is the output compare register for PB1

// lighted button and 1-channel front aux
#define AUXLED_PIN   PB4    // pin 3

// e-switch
#ifndef SWITCH_PIN
#define SWITCH_PIN   PB3    // pin 2
#define SWITCH_PCINT PCINT3 // pin 2 pin change interrupt
#endif

#define USE_VOLTAGE_DIVIDER  // use a voltage divider on pin 7, not VCC
#ifndef VOLTAGE_PIN
#define VOLTAGE_PIN      PB2    // pin 7, voltage ADC
#define VOLTAGE_CHANNEL  0x01   // MUX 01 corresponds with PB2
#define VOLTAGE_ADC      ADC1D  // Digital input disable bit corresponding with PB2
// inherited from arch/mcu.h
//#define VOLTAGE_ADC_DIDR DIDR0  // DIDR for ADC1
// 1.1V reference, left-adjust, ADC1/PB2
//#define ADMUX_VOLTAGE_DIVIDER ((1 << V_REF) | (1 << ADLAR) | VOLTAGE_CHANNEL)
// 1.1V reference, no left-adjust, ADC1/PB2
#define ADMUX_VOLTAGE_DIVIDER ((1 << V_REF) | VOLTAGE_CHANNEL)
#endif

#undef voltage_raw2cooked
#define voltage_raw2cooked  mcu_vdivider_raw2cooked

#define ADC_PRSCL   0x07    // clk/128

// Raw ADC readings at 4.4V and 2.2V (in-between, we assume values form a straight line)
#ifndef ADC_44
#define ADC_44 (234*4*4)
#endif
#ifndef ADC_22
#define ADC_22 (117*4*4)
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

