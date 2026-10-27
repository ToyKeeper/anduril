// Sofirn SC13 driver layout
// Copyright (C) 2024
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * Driver pinout:
 * eSwitch:    PA5
 * PWM:        PB0 (TCA0 WO0)
 * Reg enable: PB2
 * Voltage:    PA1 (resistors: 1M and 330K)
 * Aux Red:    PC3
 * Aux Green:  PC2
 * Aux Blue:   PC1
 
 Potentially... the battery charger (PL7203) signals get routed through the attiny
 * PB3 - CH_IN: goes high when USB power is present, otherwise pulled low
 * PA6 - CH_OK (STDBY): goes low when charging is complete
 * PB4 - green charging indicator, set high to turn on, otherwise set to low
 * PB5 - red charging indicator, set to high to turn on, otherwise set to low
 
 When CH_IN is HIGH (plugged in):
    * If CH_OK (PA6) is high (from internal pull-up), then light red indicator and turn off the green
    * If CH_OK (PA6) is low (pulled down from PL7203), then light the green indicator and turn off the red
    * turn off any aux LEDs from Anduril... what function??
    
 When CH_IN goes low (from being high):
    * turn off the red and green indicators
    * re-enabled Anduril's aux lights
 
 */

#define HWDEF_C  sofirn/sc13/hwdef.c

// allow using aux LEDs as extra channel modes
#include "fsm/chan-rgbaux.h"

// channel modes:
// * 0. main LED(s)
// * 1+. aux RGB
#define NUM_CHANNEL_MODES   (1 + NUM_RGB_AUX_CHANNEL_MODES)
enum CHANNEL_MODES {
    CM_MAIN = 0,
    RGB_AUX_ENUMS
};

#define DEFAULT_CHANNEL_MODE  CM_MAIN

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b0000000000000001


//#define PWM_CHANNELS 2  // old, remove this

#define PWM_BITS      16        // dynamic 16-bit, but never goes over 255
#define PWM_GET       PWM_GET8
#define PWM_DATATYPE  uint16_t  // is used for PWM_TOPS (which goes way over 255)
//#define PWM_DATATYPE2 uint16_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint8_t   // main output ramp
//#define PWM2_DATATYPE uint8_t   // DD FET ramp

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP TCA0.SINGLE.PERBUF   // holds the TOP value for for variable-resolution PWM
#define PWM_TOP_INIT  255    // highest value used in top half of ramp
// not necessary when double-buffered "BUF" registers are used
#define PWM_CNT TCA0.SINGLE.CNT   // for resetting phase after each TOP adjustment

// main output channel
#define CH1_PIN  PB0
#define CH1_PWM  TCA0.SINGLE.CMP0BUF  // CMP0 is the output compare register for PB0

// Buck regulator enable
#define BCK_ENABLE_PIN   PIN2_bp
#define BCK_ENABLE_PORT  PORTB_OUT

// e-switch
#define SWITCH_PIN      PIN5_bp
#define SWITCH_PORT     VPORTA.IN
#define SWITCH_ISC_REG  PORTA.PIN5CTRL
#define SWITCH_VECT     PORTA_PORT_vect
#define SWITCH_INTFLG   VPORTA.INTFLAGS

// charging circuit stuff
#define CHARGE_IND_CTRL
#define is_plugged_in    (VPORTB.IN & PIN3_bm)
#define fully_charged    (~VPORTA.IN & PIN6_bm)
#define CHARGE_IND_PORT  PORTB
#define CHARGE_IND_RED   PIN5_bp
#define CHARGE_IND_GRN   PIN4_bp

// Voltage divider battLVL
#define USE_VOLTAGE_DIVIDER       // use a dedicated pin, not VCC, because VCC input is regulated
#define ADMUX_VOLTAGE_DIVIDER  ADC_MUXPOS_AIN1_gc  // which ADC channel to read

#undef voltage_raw2cooked
#define voltage_raw2cooked  mcu_vdivider_raw2cooked

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
//   (voltage - D1) * (R2/(R2+R1) * 1024 / 1.1)
// Resistors are 1,000,000 and 300,000
#ifndef ADC_44
#define ADC_44 (4*964)  // raw value at 4.40V
#endif
#ifndef ADC_22
#define ADC_22 (4*460)  // raw value at 2.20V // was 473
#endif

// this driver allows for aux LEDs under the optic
#define AUXLED_R_PIN    PIN3_bp    // pin 3
#define AUXLED_G_PIN    PIN2_bp    // pin 2
#define AUXLED_B_PIN    PIN1_bp    // pin 1
#define AUXLED_RGB_PORT PORTC  // PORTA or PORTB or PORTC

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS


inline void hwdef_setup() {

    // set up the system clock to run at 10 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB,
                      CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );

    //VPORTA.DIR = ...;
    // Outputs: PWM and Buck Enable, charging LEDs
    VPORTB.DIR = PIN0_bm  // PWM output
               | PIN2_bm  // Buck enable
               | PIN4_bm  // charging LED: green
               | PIN5_bm; // charging LED; red
    // RGB aux LEDs
    VPORTC.DIR = PIN1_bm
               | PIN2_bm
               | PIN3_bm;

    // enable pullups on the unused pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN1CTRL = PORT_PULLUPEN_bm;  // voltage divider
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;  // eSwitch
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    //PORTB.PIN0CTRL = PORT_PULLUPEN_bm; // PWM output
    PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
    //PORTB.PIN2CTRL = PORT_PULLUPEN_bm; // Buck Enable
    //PORTB.PIN3CTRL = PORT_PULLUPEN_bm; // CH_IN is externally pulled low
    //PORTB.PIN4CTRL = PORT_PULLUPEN_bm; // charging LED: green
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm; // charging LED: green

    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
    //PORTC.PIN1CTRL = PORT_PULLUPEN_bm; // RGB Aux
    //PORTC.PIN2CTRL = PORT_PULLUPEN_bm; // RGB Aux
    //PORTC.PIN3CTRL = PORT_PULLUPEN_bm; // RGB Aux

    // set up the PWM
    // https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf
    // PB0 is TCA0:WO0, use TCA_SINGLE_CMP0EN_bm
    // PB1 is TCA0:WO1, use TCA_SINGLE_CMP1EN_bm
    // PB2 is TCA0:WO2, use TCA_SINGLE_CMP2EN_bm
    // For Fast (Single Slope) PWM use TCA_SINGLE_WGMODE_SINGLESLOPE_gc
    // For Phase Correct (Dual Slope) PWM use TCA_SINGLE_WGMODE_DSBOTTOM_gc
    // See the manual for other pins, clocks, configs, portmux, etc
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm
                      | TCA_SINGLE_WGMODE_DSBOTTOM_gc;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
                      | TCA_SINGLE_ENABLE_bm;

    PWM_TOP = PWM_TOP_INIT;

}

// set fuses, these carry over to the ELF file
// we need this for enabling BOD in Active Mode from the factory.
// settings can be verified / dumped from the ELF file using this
// command: avr-objdump -d -S -j .fuse anduril.elf
FUSES = {
    .WDTCFG  = FUSE_WDTCFG_DEFAULT,   // Watchdog Configuration
    .BODCFG  = FUSE_ACTIVE0,          // BOD Configuration
    .OSCCFG  = FUSE_OSCCFG_DEFAULT,   // Oscillator Configuration
    .TCD0CFG = FUSE_TCD0CFG_DEFAULT,  // TCD0 Configuration
    .SYSCFG0 = FUSE_SYSCFG0_DEFAULT,  // System Configuration 0
    .SYSCFG1 = FUSE_SYSCFG1_DEFAULT,  // System Configuration 1
    .APPEND  = FUSE_APPEND_DEFAULT,   // Application Code Section End
    .BOOTEND = FUSE_BOOTEND_DEFAULT,  // Boot Section End
};

#define LAYOUT_DEFINED