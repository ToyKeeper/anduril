// Copyright (C) 2017-2023 Selene ToyKeeper
//               2021-2024 loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

//*********************************************
//**  HARDWARE DEFINITIONS FOR LUME-X1-40W   **
//*********************************************

/*  Loneoceans Lume-X1-40W with ATTINY1616 (REV A1-A3)

    40W Boost Driver with UDR (Ultra Dynamic Range), RGB & Switch Aux LEDs

    The following pins are invariant (20 QFN package 3x3 0.4mm BSC)
    - PA6 / PP7  - DAC OUT
    - PA0 / PP19 - nRST / UPDI pin

    - PA3 / PP2  - EXTCLK (not used)
    - PA5 / PP6  - VREFA (not used)
    - PC2 / PP17 - NOT USED

    - PP4 / VDD - Read voltage from VDD pin, uses PFET RPP, no voltage drop

    Used Pins
    - PB3 / PP11 - Enable Pin for Boost, Op Amp Supply
    - PC3 / PP18 - E-Switch pin (no need internal pull-up)

    - PA7 / PP8  - PATH1 - Moon Range
    - PB5 / PP9  - PATH2 - Low Range
    - PB4 / PP10 - PATH3 - High Range
    - PB2 / PP12 - FET (via PWM) TCA0 - WO2 (NOT USED for Lume X1)

    - PC1 / PP16 - AUX Switch LED
    - PC0 / PP15 - AUX R LED
    - PB0 / PP14 - AUX G LED
    - PB1 / PP13 - AUX B LED

    - PA2 / PP1 / AIN2 - MIC Analog Input
    - PA1 / PP20 - MIC Enable / Supply pin
    - PA4 / PP5  - OTG pin
*/

#define HWDEF_C  loneoceans/lume-x1-40w/hwdef.c

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

//***************************************
//** SET UP DIGITAL TO ANALOG CONVERTER**
//***************************************

// Define DAC control
#define PWM_BITS      8         // 8-bit DAC
#define PWM_DATATYPE  uint8_t
#define PWM_DATATYPE2 uint16_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint8_t   // main LED ramp
#define PWM1_GET(l)   PWM_GET8(pwm1_levels, l)
#define PWM2_DATATYPE uint8_t   // DAC Vref table
#define PWM2_GET(l)   PWM_GET8(pwm2_levels, l)

//***************************************
//**         PIN DEFINITIONS           **
//***************************************

// Boost and Amplifier Enable (PB3)
#define BST_ENABLE_PIN   PIN3_bp
#define BST_ENABLE_PORT  PORTB_OUT
#define BST_ON_DELAY 8  // ms delay turning on the led after enable

// Ultra Dynamic Range (UDR)
/* 
    UDR makes use of the concept of multiple power paths, 3 are used in this
    design to achieve extremely low moonlight levels. This is combined with 
    dynamic Vref for smoother brightness level transitions that would
    normally be limited by the 8-bit DAC resolution. 

    The Lume X1 uses the internal DAC to generate a reference voltage for 
    a current-regulated amplifier which drives the LED. Each power path 
    routes current through different sense resistors, allowing for improved
    current sense resolution especially at the low end. Using UDR, Lume X1
    is capable of ultra-low firefly / moonlight levels with a dynamic range 
    on the order of 10 million : 1. 
*/

// For UDR Path 1 (firefly mode) - PA7
#define LED_PATH1_PIN PIN7_bm
#define LED_PATH1_PORT PORTA_OUT

// For UDR Path 2 (low mode) - PB5
#define LED_PATH2_PIN PIN5_bm
#define LED_PATH2_PORT PORTB_OUT

// For UDR Path 3 (high mode) - PB4
#define LED_PATH3_PIN   PIN4_bm
#define LED_PATH3_PORT  PORTB_OUT

// Define Aux LED Pins

// lighted switch button aux led (PC1)
#ifndef BUTTON_LED_PIN
#define BUTTON_LED_PIN  PIN1_bp
#define BUTTON_LED_PORT PORTC
#endif

// this driver allows for aux LEDs under the optic
#define AUXLED_R_PIN   PIN0_bp    // PC0 / PP15
#define AUXLED_G_PIN   PIN0_bp    // PB0 / PP14
#define AUXLED_B_PIN   PIN1_bp    // PB1 / PP13

#define AUXLED_R_PORT   PORTC
#define AUXLED_G_PORT   PORTB
#define AUXLED_B_PORT   PORTB

// aux leds are on different ports
#define AUXLED_RGB_DIFFERENT_PORTS 

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// Define e-switch Pin and ISR
#ifndef SWITCH_PIN   // PC3 
#define SWITCH_PIN     PIN3_bp
#define SWITCH_PORT    VPORTC.IN
#define SWITCH_ISC_REG PORTC.PIN3CTRL
#define SWITCH_VECT    PORTC_PORT_vect
#define SWITCH_INTFLG  VPORTC.INTFLAGS
#endif

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 1  // PFET for RRP, essentially 0 v-drop, but experimentally add 0.05V for better UX
#endif

// define power-bank enable pin
#define POWER_BANK_EN_PIN  PIN4_bm
#define POWER_BANK_EN_PORT PORTA_OUT

//***************************************
//**          HARDWARE INIT            **
//***************************************

inline void hwdef_setup() {

    // TODO: for this DAC controlled-light, try to decrease the clock speed
    // to reduce overall system power
    mcu_clock_speed();

    // set pins as 1 for output pins

    VPORTA.DIR = PIN4_bm | PIN6_bm | PIN7_bm;
    VPORTB.DIR = PIN0_bm | PIN1_bm | PIN3_bm | PIN4_bm | PIN5_bm;
    VPORTC.DIR = PIN0_bm;

    // set OTG pin to low during startup
    #ifdef USE_OTG_IN_MOMENTARY
    POWER_BANK_EN_PORT &= ~POWER_BANK_EN_PIN; 
    #endif

    PORTC.DIRCLR = PIN1_bm; // set Aux LED Switch Pin as input for now

    // now set pullups on input pins, and unused pins (reduce power)

    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL = PORT_PULLUPEN_bm;     // MIC Enable
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;     // MIC input
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN6CTRL = PORT_PULLUPEN_bm;  // DAC ouput
    //PORTA.PIN7CTRL = PORT_PULLUPEN_bm;  // PATH1

    //PORTB.PIN0CTRL = PORT_PULLUPEN_bm;  // AUX_G
    //PORTB.PIN1CTRL = PORT_PULLUPEN_bm;  // AUX_B
    PORTB.PIN2CTRL = PORT_PULLUPEN_bm;    // FET PWM (not used for lume X1)
    //PORTB.PIN3CTRL = PORT_PULLUPEN_bm;  // Boost / Op-amp enable pin
    //PORTB.PIN4CTRL = PORT_PULLUPEN_bm;  // PATH3
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm;  // PATH2

    //PORTC.PIN0CTRL = PORT_PULLUPEN_bm;  // AUX_R
    //PORTC.PIN1CTRL = PORT_PULLUPEN_bm;  // AUX_LED_SW

    //E-Switch (now uses external 100k pullup)
    //PORTC.PIN2CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;  // e-switch
    //PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_ISC_BOTHEDGES_gc;

    // set up the DAC (used for the switching regulator)
    // https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf
    // DAC ranges from 0V to (255 * Vref) / 256
    VREF.CTRLA |= VREF_DAC0REFSEL_2V5_gc; // also VREF_DAC0REFSEL_0V55_gc and VREF_DAC0REFSEL_1V1_gc and VREF_DAC0REFSEL_2V5_gc 
    VREF.CTRLB |= VREF_DAC0REFEN_bm;            // enable vref
    DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm;  // enable DAC
    DAC_LVL = 0;                                // lowest output during boot

}

#define LAYOUT_DEFINED