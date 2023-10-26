// BLF LT1 driver layout using the Attiny1616
// Copyright (C) 2021-2023 (gchart), Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * Driver pinout:
 * eSwitch:    PA5
 * Aux LED:    PB5
 * PWM cool:   PB0 (TCA0 WO0)
 * PWM warm:   PB1 (TCA0 WO1)
 * Voltage:    VCC
 */

#define ATTINY 1616
#include <avr/io.h>

#define HWDEF_C_FILE hwdef-blf-lt1-t1616.c

// allow using aux LEDs as extra channel modes
#include "chan-aux.h"

// channel modes:
// * 0. warm only
// * 1. cool only
// * 2. both channels, tied together, max "200%" power
// * 3. both channels, manual blend, max "100%" power
// * 4. both channels, auto blend, reversible
#define NUM_CHANNEL_MODES  6
enum channel_modes_e {
    CM_CH1 = 0,
    CM_CH2,
    CM_BOTH,
    CM_BLEND,
    CM_AUTO,
    CM_AUX
};

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b00011000
#define USE_CHANNEL_MODE_ARGS
// _, _, _, 128=middle CCT, 0=warm-to-cool
#define CHANNEL_MODE_ARGS     0,0,0,128,0,0

// can use some of the common handlers
#define USE_CALC_2CH_BLEND


#define PWM_CHANNELS   1  // old, remove this

#define PWM_BITS      8  //

#define PWM_GET       PWM_GET8
#define PWM_DATATYPE  uint8_t
#define PWM_DATATYPE2 uint16_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint8_t   //

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP_INIT  255  // highest value used in the top half of the ramp
#define PWM_TOP TCA0.SINGLE.PERBUF   // holds the TOP value for for variable-resolution PWM
#define PWM_CNT TCA0.SINGLE.CNT   // for resetting phase after each TOP adjustment
// TODO: implement DSM
#define DSM_TOP       (255<<7) // 15-bit resolution leaves 1 bit for carry

// warm LEDs
#define CH1_PIN  PB1
#define CH1_PWM TCA0.SINGLE.CMP1  // CMP1 is the output compare register for PB1

// cold LEDs
#define CH2_PIN PB0
#define CH2_PWM TCA0.SINGLE.CMP0  // CMP0 is the output compare register for PB0

// lighted button
#define AUXLED_PIN   PIN5_bp
#define AUXLED_PORT  PORTB

// e-switch
#define SWITCH_PIN     PIN5_bp
#define SWITCH_PORT    VPORTA.IN
#define SWITCH_ISC_REG PORTA.PIN2CTRL
#define SWITCH_VECT    PORTA_PORT_vect
#define SWITCH_INTFLG  VPORTA.INTFLAGS

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif


inline void hwdef_setup() {

    // set up the system clock to run at 5 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm );

    //VPORTA.DIR = ...;
    // Outputs:
    VPORTB.DIR = PIN0_bm   // cool white
               | PIN1_bm   // warm white
               | PIN5_bm;  // aux LED
    //VPORTC.DIR = ...;

    // enable pullups on the unused pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;  // eSwitch
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    //PORTB.PIN0CTRL = PORT_PULLUPEN_bm; // cold tint channel
    //PORTB.PIN1CTRL = PORT_PULLUPEN_bm; // warm tint channel
    PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm; // Aux LED

    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;

    // set up the PWM
    // https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf
    // PB0 is TCA0:WO0, use TCA_SINGLE_CMP0EN_bm
    // PB1 is TCA0:WO1, use TCA_SINGLE_CMP1EN_bm
    // For Fast (Single Slope) PWM use TCA_SINGLE_WGMODE_SINGLESLOPE_gc
    // For Phase Correct (Dual Slope) PWM use TCA_SINGLE_WGMODE_DSBOTTOM_gc
    // TODO: add references to MCU documentation
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm
                      | TCA_SINGLE_CMP1EN_bm
                      | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    PWM_TOP = PWM_TOP_INIT;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
                      | TCA_SINGLE_ENABLE_bm;
}


#define LAYOUT_DEFINED

