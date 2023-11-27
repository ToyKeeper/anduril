// Sofirn SP10 Pro pinout
// Copyright (C) 2022-2023 (original author TBD), Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * ATTINY1616 Mapping:
 * PB5 : PWM small channel (TCA0 - WO2 Alternate MUX)
 * PB3 : eSwitch
 * PB0 : PWM big channel (TCA0 - WO0)
 * PB4 : Voltage divider (ADC0 - AIN9)
 * PA1 : Boost Enable
 */

#define HWDEF_C  sofirn/sp10-pro/hwdef.c

// channel modes:
// * 0. low+high PWM stacked
#define NUM_CHANNEL_MODES  1
enum CHANNEL_MODES {
    CM_MAIN = 0,
};

#define DEFAULT_CHANNEL_MODE  CM_MAIN

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b00000001


#define PWM_CHANNELS  2  // old, remove this

#define PWM_BITS      16        // data type needs 16 bits, not 8
#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t  // is used for PWM_TOPS (which goes way over 255)
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // low PWM ramp
#define PWM2_DATATYPE uint16_t  // high PWM ramp

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP  TCA0.SINGLE.PERBUF  // holds the TOP value for for variable-resolution PWM
#define PWM_CNT  TCA0.SINGLE.CNT     // for resetting phase after each TOP adjustment
#define PWM_TOP_INIT  255            // highest value used in top half of ramp (unused?)

// Small channel
#define CH1_PIN  PB5
#define CH1_PWM  TCA0.SINGLE.CMP2BUF  // PB5 is Alternate MUX for TCA Compare 2

// Big channel
#define CH2_PIN  PB0
#define CH2_PWM  TCA0.SINGLE.CMP0BUF  // PB0 is TCA Compare 0

// boost enable
#define BST_ENABLE_PIN   PIN1_bp
#define BST_ENABLE_PORT  PORTA_OUT

// e-switch
#define SWITCH_PIN      3
#define SWITCH_PORT     VPORTB.IN
#define SWITCH_ISC_REG  PORTB.PIN3CTRL
#define SWITCH_VECT     PORTB_PORT_vect
#define SWITCH_INTFLG   VPORTB.INTFLAGS

// Voltage divider battLVL
#define USE_VOLTAGE_DIVIDER       // use a dedicated pin, not VCC, because VCC input is regulated
#define DUAL_VOLTAGE_FLOOR     (4*21) // for AA/14500 boost drivers, don't indicate low voltage if below this level
#define DUAL_VOLTAGE_LOW_LOW   (4*7)  // the lower voltage range's danger zone 0.7 volts (NiMH)
#define ADMUX_VOLTAGE_DIVIDER  ADC_MUXPOS_AIN9_gc  // which ADC channel to read

#undef voltage_raw2cooked
#define voltage_raw2cooked  mcu_vdivider_raw2cooked

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
//   (voltage - D1) * (R2/(R2+R1) * 1024 / 1.1)
// Resistors are 300,000 and 100,000
#ifndef ADC_44
#define ADC_44 (4*1023)  // raw value at 4.40V
#endif
#ifndef ADC_22
#define ADC_22 (4*512)  // raw value at 2.20V
#endif



inline void hwdef_setup() {

    // set up the system clock to run at 10 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB,
                      CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );

    VPORTA.DIR = PIN1_bm;  // Boost enable pin
    VPORTB.DIR = PIN0_bm   // big PWM channel
               | PIN5_bm;  // small PWM channel
    //VPORTC.DIR = ...;

    // enable pullups on the input pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN1CTRL = PORT_PULLUPEN_bm; // Boost enable pin
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    //PORTB.PIN0CTRL = PORT_PULLUPEN_bm; // Big PWM channel
    PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN3CTRL = PORT_PULLUPEN_bm
                   | PORT_ISC_BOTHEDGES_gc;  // e-switch
    //PORTB.PIN4CTRL = PORT_PULLUPEN_bm; // Voltage divider
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm; // Small PWM channel

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
    PORTMUX.CTRLC = PORTMUX_TCA02_ALTERNATE_gc;  // Use alternate pin for TCA0:WO2
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm
                      | TCA_SINGLE_CMP2EN_bm
                      | TCA_SINGLE_WGMODE_DSBOTTOM_gc;
    PWM_TOP = PWM_TOP_INIT;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
                      | TCA_SINGLE_ENABLE_bm;
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

