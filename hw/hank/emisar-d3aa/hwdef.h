// hwdef for Emisar D3AA
// Copyright (C) 2023 thefreeman, Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * NiMH/li-ion 9V2A boost driver based on MP3432
 * with high dynamic range and DAC control + front aux RGB and button LED
 *
 * Pin   Name   Function
 *   1   PA4    e-switch
 *   2   PA5    BATT LVL (voltage divider)
 *   3   PA6    EN: boost enable
 *   4   PA7    A : button LED
 *   5   PC1    -
 *   6   PC2    -
 *   7   PC3    -
 *   8   VDDIO2 (BATT+ via solder jumper)
 *   9   PD4    IN- NFET: absorb startup flash
 *  10   PD5    HDR: high/low Rsense range
 *  11   PD6    DAC: control voltage out
 *  12   PD7    
 *  13   VDD    VCC
 *  14   GND    GND
 *  15   PF6    RESET
 *  16   PF7    UPDI
 *  17   PA0    B: aux blue
 *  18   PA1    
 *  19   PA2    G: aux green
 *  20   PA3    R: aux red
 *
 * BST EN enable the boost regulator and Op-Amp
 * DAC sets the current, max current depends on Vset voltage divider and Rsense
 * HDR FET switches between high value Rsense (low current range, pin low),
 *         and low value Rsense (high current range, pin high)
 * IN- NFET : pull up after BST enable to eliminate startup flash, pull down otherwise
 * BATT LVL : Vbat * (100.0/(330+100))
 * VDDIO2 : can be connected to BATT+ with a solder jumper for VDDIO2 voltage sensing
 *
 */

#define HWDEF_C  hank/emisar-d3aa/hwdef.c

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


// DAC max is 1023, Anduril is written for 255, so regulate at 4X speed
#undef  GRADUAL_ADJUST_SPEED
#define GRADUAL_ADJUST_SPEED  4

#define PWM_BITS      16        // 10-bit DAC
#define PWM_DATATYPE  uint16_t
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // main LED ramp
#define PWM1_GET(l)   PWM_GET16(pwm1_levels, l)
#define PWM2_DATATYPE uint8_t   // DAC Vref table
#define PWM2_GET(l)   PWM_GET8(pwm2_levels, l)

// main LED outputs
// (DAC_LVL + DAC_VREF + Vref values are defined in arch/*.h)

// BST enable
#define BST_ENABLE_PIN   PIN6_bp
#define BST_ENABLE_PORT  PORTA_OUT

// HDR
// turns on HDR FET for the high current range
#define HDR_ENABLE_PIN   PIN5_bp
#define HDR_ENABLE_PORT  PORTD_OUT

// IN- NFET
// pull high to force output to zero to eliminate the startup flash
#define IN_NFET_DELAY_TIME   12  // (ms)
#define IN_NFET_ENABLE_PIN   PIN4_bp
#define IN_NFET_ENABLE_PORT  PORTD_OUT

// e-switch
#ifndef SWITCH_PIN
#define SWITCH_PIN      PIN4_bp
#define SWITCH_PORT     VPORTA.IN
#define SWITCH_ISC_REG  PORTA.PIN4CTRL
#define SWITCH_VECT     PORTA_PORT_vect
#define SWITCH_INTFLG   VPORTA.INTFLAGS
#endif

#define DUAL_VOLTAGE_FLOOR     (21*dV)  // for AA/14500 boost drivers, don't indicate low voltage if below this level
#define DUAL_VOLTAGE_LOW_LOW   ( 7*dV)  // the lower voltage range's danger zone 0.7 volts (NiMH)
// comment out to use VDDIO2 instead of external voltage divider
#define USE_VOLTAGE_DIVIDER
#ifdef USE_VOLTAGE_DIVIDER
    // AVR datasheet table 3.1 I/O Multiplexing, PA5 ADC0 = AIN25
    #define ADMUX_VOLTAGE_DIVIDER  ADC_MUXPOS_AIN25_gc
    // don't use the default VDD converter
    // convert BATT LVL pin readings to FSM volt units
    #undef voltage_raw2cooked
    uint8_t voltage_raw2cooked(uint16_t measurement);
#else
    // doesn't work on this hardware in AA mode
    #define USE_VOLTAGE_VDDIO2
#endif

// Alkaline AA can't handle the power this light wants,
// so try to detect it and limit the maximum power
// (also helps protect firmware flashing adapters from overload)
#define USE_RAMP_LEVEL_HARD_LIMIT
#define USE_WEAK_BATTERY_PROTECTION
// define this next to the ramp table instead
//#define WEAK_BATTERY_TEST_MAX_LEVEL       75  // about 300 mA
#define WEAK_BATTERY_SAG_THRESHOLD_AA     (3*4)  // 0.3 V
#define WEAK_BATTERY_SAG_THRESHOLD_LIION  (6*4)  // 0.6 V

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 0  // using a PFET so no appreciable drop
#endif

// this driver allows for aux LEDs under the optic
#define AUXLED_R_PIN  PIN3_bp
#define AUXLED_G_PIN  PIN2_bp
#define AUXLED_B_PIN  PIN0_bp
#define AUXLED_RGB_PORT PORTA

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// A: button LED
#ifndef BUTTON_LED_PIN
#define BUTTON_LED_PIN  PIN7_bp
#define BUTTON_LED_PORT PORTA
#endif


inline void hwdef_setup() {

    // TODO: for this DAC controlled-light, try to decrease the clock speed
    // to reduce overall system power
    mcu_clock_speed();

    VPORTA.DIR = PIN0_bm   // B
               | PIN2_bm   // G
               | PIN3_bm   // R
               | PIN6_bm   // EN
               | PIN7_bm;  // A
    VPORTD.DIR = PIN4_bm   // IN- NFET
               | PIN5_bm   // HDR
               | PIN6_bm;  // DAC

    // enable pullups on the unused and input pins to reduce power
    //PORTA.PIN0CTRL = PORT_PULLUPEN_bm;  // B
    PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN2CTRL = PORT_PULLUPEN_bm;  // G
    //PORTA.PIN3CTRL = PORT_PULLUPEN_bm;  // R
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm
                   | PORT_ISC_BOTHEDGES_gc;  // e-switch
    //PORTA.PIN5CTRL = PORT_PULLUPEN_bm;  // BATT LVL
    //PORTA.PIN6CTRL = PORT_PULLUPEN_bm;  // EN
    //PORTA.PIN7CTRL = PORT_PULLUPEN_bm;  // A

    //PORTC.PIN0CTRL = PORT_PULLUPEN_bm;  // doesn't exist
    PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
    //PORTC.PIN4CTRL = PORT_PULLUPEN_bm;  // doesn't exist
    //PORTC.PIN5CTRL = PORT_PULLUPEN_bm;  // doesn't exist

    //PORTD.PIN0CTRL = PORT_PULLUPEN_bm;  // doesn't exist
    //PORTD.PIN1CTRL = PORT_PULLUPEN_bm;  // doesn't exist
    //PORTD.PIN2CTRL = PORT_PULLUPEN_bm;  // doesn't exist
    //PORTD.PIN3CTRL = PORT_PULLUPEN_bm;  // doesn't exist
    //PORTD.PIN4CTRL = PORT_PULLUPEN_bm;  // IN- NFET
    //PORTD.PIN5CTRL = PORT_PULLUPEN_bm;  // EN
    // AVR datasheet 34.3.1 #2, DAC pin must have input disable set
    PORTD.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;  // DAC
    PORTD.PIN7CTRL = PORT_PULLUPEN_bm;

    // set up the DAC
    // DAC ranges from 0V to (255 * Vref) / 256
    DAC_VREF = V10;
    // TODO: try DAC_RUNSTDBY_bm for extra-efficient moon
    DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm;
    DAC_LVL = 0;  // turn off output at boot
    // TODO: instead of enabling the DAC at boot, pull pin down
    //       to generate a zero without spending power on the DAC
    //       (and do this in set_level_zero() too)

}


// set fuses, these carry over to the ELF file
// we need this for enabling BOD in Active Mode from the factory.
// settings can be verified / dumped from the ELF file using this
// command: avr-objdump -d -S -j .fuse anduril.elf
FUSES = {
    .WDTCFG  = FUSE_WDTCFG_DEFAULT,   // Watchdog Configuration

    // enable BOD (continuous) in active mode
    .BODCFG  = ACTIVE_ENABLE_gc,      // BOD Configuration

    .OSCCFG  = FUSE_OSCCFG_DEFAULT,   // Oscillator Configuration
    .SYSCFG0 = FUSE_SYSCFG0_DEFAULT,  // System Configuration 0

    // enable MVIO because VDDIO2 pin isn't connected
    // set startup time to 64ms to allow power to stabilize
    .SYSCFG1 = MVSYSCFG_DUAL_gc | SUT_64MS_gc,

    .CODESIZE = FUSE_CODESIZE_DEFAULT,
    .BOOTSIZE = FUSE_BOOTSIZE_DEFAULT,
};


#define LAYOUT_DEFINED

