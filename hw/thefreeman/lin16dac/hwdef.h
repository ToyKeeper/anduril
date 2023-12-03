// thefreeman's Linear 16 driver using DAC control
// Copyright (C) 2021-2023 thefreeman, Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * PA6 - DAC for LED brightness control
 * PA7 - Op-amp enable pin
 * PB5 - Aux LED
 * PB4 - Switch pin, internal pullup
 * PB3 - HDR control, set High to enable the high power channel, set Low for low power
 * Read voltage from VCC pin, has PFET so no drop
 */

#define HWDEF_C  thefreeman/lin16dac/hwdef.c

// allow using aux LEDs as extra channel modes
#include "fsm/chan-aux.h"

// channel modes:
// * 0. main LEDs
// * 1+. aux RGB
#define NUM_CHANNEL_MODES  2
enum CHANNEL_MODES {
    CM_MAIN = 0,
    CM_AUX
};

#define DEFAULT_CHANNEL_MODE  CM_MAIN

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b0000000000000001


#define PWM_BITS      8         // 8-bit DAC
#define PWM_DATATYPE  uint8_t
#define PWM_DATATYPE2 uint16_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint8_t   // main LED ramp
#define PWM1_GET(l)   PWM_GET8(pwm1_levels, l)
#define PWM2_DATATYPE uint8_t   // DAC Vref table
#define PWM2_GET(l)   PWM_GET8(pwm2_levels, l)

// main LED outputs
// (DAC_LVL + DAC_VREF + Vref values are defined in arch/*.h)

// Opamp enable
// For turning on and off the op-amp
#define OPAMP_ENABLE_PIN   PIN7_bp
#define OPAMP_ENABLE_PORT  PORTA_OUT
// how many ms to delay turning on the lights after enabling the channel
// (FIXME: 80 is long enough it's likely to cause bugs elsewhere,
//  as events stack up unhandled for 5 consecutive WDT ticks)
#define OPAMP_ON_DELAY 80

// HDR
// turns on HDR FET for the high current range
#define HDR_ENABLE_PIN   PIN3_bp
#define HDR_ENABLE_PORT  PORTB_OUT

// e-switch
#define SWITCH_PIN      PIN4_bp
#define SWITCH_PORT     VPORTB.IN
#define SWITCH_ISC_REG  PORTB.PIN2CTRL
#define SWITCH_VECT     PORTB_PORT_vect
#define SWITCH_INTFLG   VPORTB.INTFLAGS

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 0  // using a PFET so no appreciable drop
#endif

// lighted button
#define AUXLED_PIN   PIN5_bp
#define AUXLED_PORT  PORTB


inline void hwdef_setup() {

    // TODO: for this DAC controlled-light, try to decrease the clock speed
    // to reduce overall system power
    mcu_clock_speed();

    VPORTA.DIR = PIN6_bm   // DAC
               | PIN7_bm;  // Opamp
    VPORTB.DIR = PIN3_bm;  // HDR
    //VPORTC.DIR = 0b00000000;

    // enable pullups on the input pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN6CTRL = PORT_PULLUPEN_bm;  // DAC ouput
    //PORTA.PIN7CTRL = PORT_PULLUPEN_bm;  // Op-amp enable pin

    PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
    //PORTB.PIN3CTRL = PORT_PULLUPEN_bm;  // HDR channel selection
    PORTB.PIN4CTRL = PORT_PULLUPEN_bm
                   | PORT_ISC_BOTHEDGES_gc;  // e-switch
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm;  // Aux LED

    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;

    // set up the DAC
    // https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf
    // DAC ranges from 0V to (255 * Vref) / 256
    mcu_set_dac_vref(V05);  // boot at lowest Vref setting
    VREF.CTRLB |= VREF_DAC0REFEN_bm;  // enable DAC Vref
    DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm;  // enable DAC
    DAC_LVL = 0;  // turn off output at boot

}


#define LAYOUT_DEFINED

