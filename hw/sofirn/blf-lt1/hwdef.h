// BLF LT1 driver layout
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 *           ----
 *   Reset -|1  8|- VCC
 * eswitch -|2  7|- (unused)
 * aux LED -|3  6|- PWM (5000K)
 *     GND -|4  5|- PWM (3000K)
 *           ----
 */

#define HWDEF_C  sofirn/blf-lt1/hwdef.c

// channel modes:
// * 0. channel 1 only
// * 1. channel 2 only
// * 2. both channels, tied together, max "200%" power
// * 3. both channels, manual blend, max "100%" power
// * 4. both channels, auto blend, reversible
#define NUM_CHANNEL_MODES   5
enum channel_modes_e {
    CM_CH1 = 0,
    CM_CH2,
    CM_BOTH,
    CM_BLEND,
    CM_AUTO,
};


// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b00011000
#define USE_CHANNEL_MODE_ARGS
// _, _, _, 128=middle CCT, 0=warm-to-cool
#define CHANNEL_MODE_ARGS     0,0,0,128,0

// can use some of the common handlers
#define USE_CALC_2CH_BLEND


#define PWM_CHANNELS   1  // old, remove this

#define PWM_BITS      16  // 8-bit hardware PWM + 16-bit DSM

#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // 15-bit PWM+DSM ramp

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP_INIT  255
#define DSM_TOP       (255<<7) // 15-bit resolution leaves 1 bit for carry

// timer interrupt for DSM
#define DSM_vect     TIMER0_OVF_vect
#define DSM_INTCTRL  TIMSK
#define DSM_OVF_bm   (1<<TOIE0)

#define DELAY_FACTOR 90  // less time in delay() because more time spent in interrupts

// warm LEDs
uint16_t ch1_dsm_lvl;
uint8_t ch1_pwm, ch1_dsm;
#define CH1_PIN  PB1        // pin 6, warm tint PWM
#define CH1_PWM  OCR0B      // OCR0B is the output compare register for PB1

// cold LEDs
uint16_t ch2_dsm_lvl;
uint8_t ch2_pwm, ch2_dsm;
#define CH2_PIN  PB0        // pin 5, cold tint PWM
#define CH2_PWM  OCR0A      // OCR0A is the output compare register for PB0

// lighted button
#define AUXLED_PIN   PB4    // pin 3

// e-switch
#define SWITCH_PIN   PB3    // pin 2
#define SWITCH_PCINT PCINT3 // pin 2 pin change interrupt

#define ADC_PRSCL   0x07    // clk/128

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels


inline void hwdef_setup() {
    // configure PWM channels
    DDRB = (1 << CH1_PIN)
         | (1 << CH2_PIN);

    TCCR0B = 0x01; // pre-scaler for timer (1 => 1, 2 => 8, 3 => 64...)
    TCCR0A = PHASE;

    // enable timer overflow interrupt for DSM purposes
    //DSM_INTCTRL |= DSM_OVF_bm;  // moved to hwdef.c functions instead

    // configure e-switch
    PORTB = (1 << SWITCH_PIN);  // e-switch is the only input
    PCMSK = (1 << SWITCH_PIN);  // pin change interrupt uses this pin
}


#define LAYOUT_DEFINED

