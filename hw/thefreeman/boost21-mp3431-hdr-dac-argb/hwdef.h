// hwdef for thefreeman's boost driver 2.1 w/ MP3431, DAC, ARGB
// Copyright (C) 2023 TBD (thefreeman), Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/* thefreeman’s Boost driver based on MP3431 and attiny1616
 * with high dynamic range and DAC control, AUX : RGB + button
 * hardware version : 2.0+
 * compatible with BST20-FWxA v1.0 (no button LED A)
 *
 * Pin / Name / Function
 *   1    PA2   
 *   2    PA3   
 *   3    GND   GND
 *   4    VCC   VCC
 *   5    PA4   
 *   6    PA5   
 *   7    PA6   DAC
 *   8    PA7   
 *   9    PB5   IN- NFET
 *  10    PB4   HDR
 *  11    PB3   B: blue aux LED
 *  12    PB2   G: green aux LED
 *  13    PB1   R: red aux LED
 *  14    PB0   
 *  15    PC0   boost enable
 *  16    PC1   A: button LED
 *  17    PC2   e-switch
 *  18    PC3   
 *  19    PA0   UDPI
 *  20    PA1   
 *
 * BST EN enable the boost regulator and Op-Amp
 * DAC sets the current, max current depends on Vset voltage divider and Rsense
 * HDR FET switches between high value Rsense (low current range, pin low),
 *         and low value Rsense (high current range, pin high)
 * IN- NFET : pull up after BST enable to eliminate startup flash, pull down otherwise
 */

#define HWDEF_C  thefreeman/boost21-mp3431-hdr-dac-argb/hwdef.c

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


#define PWM_BITS      8         // 8-bit DAC
#define PWM_DATATYPE  uint8_t
#define PWM_DATATYPE2 uint16_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint8_t   // main LED ramp
#define PWM1_GET(l)   PWM_GET8(pwm1_levels, l)
#define PWM2_DATATYPE uint8_t   // DAC Vref table
#define PWM2_GET(l)   PWM_GET8(pwm2_levels, l)

// main LED outputs
// (DAC_LVL + DAC_VREF + Vref values are defined in arch/*.h)

// BST enable
#define BST_ENABLE_PIN   PIN0_bp
#define BST_ENABLE_PORT  PORTC_OUT

// HDR
// turns on HDR FET for the high current range
#define HDR_ENABLE_PIN   PIN4_bp
#define HDR_ENABLE_PORT  PORTB_OUT

// IN- NFET
// pull high to force output to zero to eliminate the startup flash
#define IN_NFET_DELAY_TIME   8  // (ms)
#define IN_NFET_ENABLE_PIN   PIN5_bp
#define IN_NFET_ENABLE_PORT  PORTB_OUT

// e-switch
#ifndef SWITCH_PIN
#define SWITCH_PIN      PIN2_bp
#define SWITCH_PORT     VPORTC.IN
#define SWITCH_ISC_REG  PORTC.PIN2CTRL
#define SWITCH_VECT     PORTC_PORT_vect
#define SWITCH_INTFLG   VPORTC.INTFLAGS
#endif

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 0  // using a PFET so no appreciable drop
#endif

// this driver allows for aux LEDs under the optic
#define AUXLED_R_PIN  PIN1_bp
#define AUXLED_G_PIN  PIN2_bp
#define AUXLED_B_PIN  PIN3_bp
#define AUXLED_RGB_PORT PORTB  // PORTA or PORTB or PORTC

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// A: button LED
#ifndef BUTTON_LED_PIN
#define BUTTON_LED_PIN  PIN1_bp
#define BUTTON_LED_PORT PORTC
#endif


inline void hwdef_setup() {

    // TODO: for this DAC controlled-light, try to decrease the clock speed
    // to reduce overall system power
    mcu_clock_speed();

    VPORTA.DIR = PIN6_bm;  // DAC
    VPORTB.DIR = PIN1_bm   // R
               | PIN2_bm   // G
               | PIN3_bm   // B
               | PIN4_bm   // HDR
               | PIN5_bm;  // IN- NFET
    VPORTC.DIR = PIN0_bm   // BST EN
               | PIN1_bm;  // A

    // enable pullups on the input pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN6CTRL = PORT_PULLUPEN_bm;  // DAC
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
    //PORTB.PIN1CTRL = PORT_PULLUPEN_bm;  // R
    //PORTB.PIN2CTRL = PORT_PULLUPEN_bm;  // G
    //PORTB.PIN3CTRL = PORT_PULLUPEN_bm;  // B
    //PORTB.PIN4CTRL = PORT_PULLUPEN_bm;  // HDR
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm;  // IN- NFET

    //PORTC.PIN0CTRL = PORT_PULLUPEN_bm;  // EN
    //PORTC.PIN1CTRL = PORT_PULLUPEN_bm;  // A
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm
                   | PORT_ISC_BOTHEDGES_gc;  // e-switch
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

