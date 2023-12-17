// BLF Q8 driver layout using the Attiny1616
// Copyright (C) 2021-2023 gchart, Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * (based on Wurkkos TS10 driver layout,
 *  which in turn was based on an older version of this BLF-Q8-t1616 driver)
 * (should probably merge the two files at some point)
 * Driver pinout:
 * eSwitch:    PA5
 * Aux LED:    PB5
 * PWM FET:    PB0 (TCA0 WO0)
 * PWM 1x7135: PB1 (TCA0 WO1)
 * Voltage:    VCC
 */

// nearly all t1616-based FET+1 drivers work pretty much the same
// (this one has single-color aux like the TS10)
#define HWDEF_C  wurkkos/ts10/hwdef.c

// allow using aux LEDs as extra channel modes
#include "fsm/chan-aux.h"

// channel modes:
// * 0. FET+7135 stacked
// * 1. aux LEDs
#define NUM_CHANNEL_MODES  2
enum CHANNEL_MODES {
    CM_MAIN = 0,
    CM_AUX
};

#define DEFAULT_CHANNEL_MODE  CM_MAIN

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b00000001


#define PWM_CHANNELS 2  // old, remove this

#define PWM_BITS      16        // dynamic 16-bit, but never goes over 255
#define PWM_GET       PWM_GET8
#define PWM_DATATYPE  uint16_t  // is used for PWM_TOPS (which goes way over 255)
#define PWM_DATATYPE2 uint16_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint8_t   // 1x7135 ramp
#define PWM2_DATATYPE uint8_t   // DD FET ramp

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP TCA0.SINGLE.PERBUF   // holds the TOP value for for variable-resolution PWM
#define PWM_TOP_INIT  255    // highest value used in top half of ramp
// not necessary when double-buffered "BUF" registers are used
#define PWM_CNT TCA0.SINGLE.CNT   // for resetting phase after each TOP adjustment

// 1x7135 channel
#define CH1_PIN  PB1
#define CH1_PWM  TCA0.SINGLE.CMP1BUF  // CMP1 is the output compare register for PB1

// DD FET channel
#define CH2_PIN  PB0
#define CH2_PWM  TCA0.SINGLE.CMP0BUF  // CMP0 is the output compare register for PB0

// e-switch
#define SWITCH_PIN      PIN5_bp
#define SWITCH_PORT     VPORTA.IN
#define SWITCH_ISC_REG  PORTA.PIN2CTRL
#define SWITCH_VECT     PORTA_PORT_vect
#define SWITCH_INTFLG   VPORTA.INTFLAGS

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

// lighted button
#define AUXLED_PIN   PIN5_bp
#define AUXLED_PORT  PORTB


inline void hwdef_setup() {

    // set up the system clock to run at 10 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB,
                      CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );

    //VPORTA.DIR = ...;
    // Outputs
    VPORTB.DIR = PIN0_bm   // DD FET
               | PIN1_bm   // 7135
               | PIN5_bm;  // Aux LED
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

    //PORTB.PIN0CTRL = PORT_PULLUPEN_bm; // FET channel
    //PORTB.PIN1CTRL = PORT_PULLUPEN_bm; // 7135 channel
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
    // PB2 is TCA0:WO2, use TCA_SINGLE_CMP2EN_bm
    // For Fast (Single Slope) PWM use TCA_SINGLE_WGMODE_SINGLESLOPE_gc
    // For Phase Correct (Dual Slope) PWM use TCA_SINGLE_WGMODE_DSBOTTOM_gc
    // See the manual for other pins, clocks, configs, portmux, etc
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm
                      | TCA_SINGLE_CMP1EN_bm
                      | TCA_SINGLE_WGMODE_DSBOTTOM_gc;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
                      | TCA_SINGLE_ENABLE_bm;

    PWM_TOP = PWM_TOP_INIT;

}


#define LAYOUT_DEFINED

