// hwdef for thefreeman's boost FWAA driver 1.1 w/ MP3432, HDR DAC, RGB aux
// Copyright (C) 2023 TBD (thefreeman), Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/* thefreeman’s FWAA AA/li-ion Boost driver based on MP3432 and attiny1616
 * with high dynamic range and DAC control, AUX : RGB
 * hardware versions : 1.0, 1.1
 *
 * Pin / Name / Function
 *   1    PA2   BattLVL (ADC0 - AIN2)
 *   2    PA3   
 *   3    GND   GND
 *   4    VCC   VCC
 *   5    PA4   BST EN: boost enable
 *   6    PA5   HDR
 *   7    PA6   DAC
 *   8    PA7   
 *   9    PB5   B: blue aux LED
 *  10    PB4   G: green aux LED
 *  11    PB3   R: red aux LED
 *  12    PB2   IN- NFET
 *  13    PB1   
 *  14    PB0   
 *  15    PC0   
 *  16    PC1   
 *  17    PC2   
 *  18    PC3   e-switch
 *  19    PA0   UDPI
 *  20    PA1   
 *
 * BST EN enable the boost regulator and Op-Amp
 * DAC sets the current, max current depends on Vset voltage divider and Rsense
 * HDR FET switches between high value Rsense (low current range, pin low),
 *         and low value Rsense (high current range, pin high)
 * IN- NFET : pull up after BST enable to eliminate startup flash, pull down otherwise
 */

#define ATTINY 1616
#include <avr/io.h>

#define HWDEF_C_FILE hwdef-thefreeman-boost21-mp3431-hdr-dac-argb.c

// allow using aux LEDs as extra channel modes
#include "chan-rgbaux.h"

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


#define PWM_CHANNELS  1  // old, remove this

#define PWM_BITS      8         // 8-bit DAC
#define PWM_GET       PWM_GET8
#define PWM_DATATYPE  uint8_t
#define PWM_DATATYPE2 uint16_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint8_t   // main LED ramp

// main LED outputs
#define DAC_LVL   DAC0.DATA    // 0 to 255, for 0V to Vref
#define DAC_VREF  VREF.CTRLA   // 0.55V or 2.5V
#define PWM_TOP_INIT  255      // highest value used in top half of ramp (unused?)
// Vref values
#define V055  16
#define V11   17
#define V25   18
#define V43   19
#define V15   20

// BST enable
#define BST_ENABLE_PIN   PIN4_bp
#define BST_ENABLE_PORT  PORTA_OUT

// HDR
// turns on HDR FET for the high current range
#define HDR_ENABLE_PIN   PIN5_bp
#define HDR_ENABLE_PORT  PORTA_OUT

// IN- NFET
// pull high to force output to zero to eliminate the startup flash
#define IN_NFET_DELAY_TIME   4  // (ms)
#define IN_NFET_ENABLE_PIN   PIN2_bp
#define IN_NFET_ENABLE_PORT  PORTB_OUT

// e-switch
#ifndef SWITCH_PIN
#define SWITCH_PIN      PIN3_bp
#define SWITCH_PORT     VPORTC.IN
#define SWITCH_ISC_REG  PORTC.PIN3CTRL
#define SWITCH_VECT     PORTC_PORT_vect
#define SWITCH_INTFLG   VPORTC.INTFLAGS
#define SWITCH_PCINT    PCINT0
#define PCINT_vect      PCINT0_vect
#endif

// Voltage divider battLVL
#define USE_VOLTAGE_DIVIDER       // use a dedicated pin, not VCC, because VCC input is regulated
#define DUAL_VOLTAGE_FLOOR     21  // for AA/14500 boost drivers, don't indicate low voltage if below this level
#define DUAL_VOLTAGE_LOW_LOW   7  // the lower voltage range's danger zone 0.7 volts (NiMH)
#define ADMUX_VOLTAGE_DIVIDER  ADC_MUXPOS_AIN2_gc  // which ADC channel to read

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
//   (voltage - D1) * (R2/(R2+R1) * 1024 / 1.1)
// Resistors are 330k and 100k
#ifndef ADC_44
#define ADC_44 951  // raw value at 4.40V
#endif
#ifndef ADC_22
#define ADC_22 476  // raw value at 2.20V
#endif

// this driver allows for aux LEDs under the optic
#define AUXLED_R_PIN  PIN3_bp
#define AUXLED_G_PIN  PIN4_bp
#define AUXLED_B_PIN  PIN5_bp
#define AUXLED_RGB_PORT PORTB  // PORTA or PORTB or PORTC

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS


inline void hwdef_setup() {

    // TODO: for this DAC controlled-light, try to decrease the clock speed or use the ULP
    // set up the system clock to run at 10 MHz to match other attiny1616 lights
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB,
                      CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );

    VPORTA.DIR = PIN4_bm  // BST EN
               | PIN5_bm  // HDR
               | PIN6_bm; // DAC
    VPORTB.DIR = PIN2_bm  // IN- NFET
               | PIN3_bm  // R
               | PIN4_bm  // G
               | PIN5_bm; // B
    //VPORTC.DIR = PIN0_bm | PIN1_bm;

    // enable pullups on the input pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN2CTRL = PORT_PULLUPEN_bm;  // BattLVL
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN4CTRL = PORT_PULLUPEN_bm;  // EN
    //PORTA.PIN5CTRL = PORT_PULLUPEN_bm;  // HDR
    //PORTA.PIN6CTRL = PORT_PULLUPEN_bm;  // DAC
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
    
    PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
    //PORTB.PIN2CTRL = PORT_PULLUPEN_bm;  // IN- NFET
    //PORTB.PIN3CTRL = PORT_PULLUPEN_bm;  // R
    //PORTB.PIN4CTRL = PORT_PULLUPEN_bm;  // G
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm;  // B
    
    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm
                   | PORT_ISC_BOTHEDGES_gc;  // e-switch

    // set up the DAC
    // https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf
    // DAC ranges from 0V to (255 * Vref) / 256
    // also VREF_DAC0REFSEL_0V55_gc and VREF_DAC0REFSEL_1V1_gc and VREF_DAC0REFSEL_2V5_gc
    VREF.CTRLA |= VREF_DAC0REFSEL_2V5_gc;
    VREF.CTRLB |= VREF_DAC0REFEN_bm;
    DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm;
    DAC0.DATA = 255; // set the output voltage

}


#define LAYOUT_DEFINED

