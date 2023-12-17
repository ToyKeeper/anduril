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

#define HWDEF_C  sofirn/blf-lt1-t1616/hwdef.c

// allow using aux LEDs as extra channel modes
#include "fsm/chan-aux.h"

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


#define PWM_CHANNELS  1  // old, remove this

#define PWM_BITS      16     // 0 to 32640 (0 to 255 PWM + 0 to 127 DSM) at constant kHz
#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // 15-bit PWM+DSM ramp

// PWM parameters of both channels are tied together because they share a counter
// dynamic PWM
#define PWM_TOP  TCA0.SINGLE.PERBUF  // holds the TOP value for for variable-resolution PWM
#define PWM_TOP_INIT  255
#define PWM_CNT  TCA0.SINGLE.CNT  // for resetting phase after each TOP adjustment
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
#define DSM_TOP       (255<<7) // 15-bit resolution leaves 1 bit for carry

// timer interrupt for DSM
#define DSM_vect     TCA0_OVF_vect
#define DSM_INTCTRL  TCA0.SINGLE.INTCTRL
#define DSM_INTFLAGS TCA0.SINGLE.INTFLAGS
#define DSM_OVF_bm   TCA_SINGLE_OVF_bm

#define DELAY_FACTOR 90  // less time in delay() because more time spent in interrupts

// warm LEDs
uint16_t ch1_dsm_lvl;
uint8_t ch1_pwm, ch1_dsm;
#define CH1_PIN  PB1
#define CH1_PWM  TCA0.SINGLE.CMP1BUF  // CMP1 is the output compare register for PB1

// cold LEDs
uint16_t ch2_dsm_lvl;
uint8_t ch2_pwm, ch2_dsm;
#define CH2_PIN  PB0
#define CH2_PWM  TCA0.SINGLE.CMP0BUF  // CMP0 is the output compare register for PB0

// lighted button
#define AUXLED_PIN   PIN5_bp
#define AUXLED_PORT  PORTB

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


inline void hwdef_setup() {

    // set up the system clock to run at 10 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB,
                      CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );

    //VPORTA.DIR = ...;
    // Outputs
    VPORTB.DIR = PIN0_bm   // cool white
               | PIN1_bm   // warm white
    //           | PIN2_bm   // for testing on LT1S Pro, disable red channel
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
    PORTB.PIN2CTRL = PORT_PULLUPEN_bm; // comment out for testing on LT1S Pro
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
                      | TCA_SINGLE_WGMODE_DSBOTTOM_gc;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
                      | TCA_SINGLE_ENABLE_bm;

    PWM_TOP = PWM_TOP_INIT;

    // set up interrupt for delta-sigma modulation
    // (moved to hwdef.c functions so it can be enabled/disabled based on ramp level)
    //DSM_INTCTRL |= DSM_OVF_bm;  // interrupt once for each timer cycle

}


#define LAYOUT_DEFINED

