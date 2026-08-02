// Wurkkos TS26 driver layout
// Copyright (C) 2023-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/* Boost driver based on MP3432 and attiny1616
 * with high dynamic range and DAC control, AUX : RGB + button
 * hardware version : 2.0+
 * compatible with BST20-FWxA v1.0 (no button LED A)
 *
 * Pin / Name / Function
 *   1    PA2   -
 *   2    PA3   -
 *   3    GND   GND
 *   4    VCC   VCC
 *   5    PA4   -
 *   6    PA5   e-switch
 *   7    PA6   -
 *   8    PA7   -
 *   9    PB5   -
 *  10    PB4   -
 *  11    PB3   -
 *  12    PB2   EN: boost enable
 *  13    PB1   -
 *  14    PB0   PWM
 *  15    PC0   -
 *  16    PC1   B: aux blue
 *  17    PC2   G: aux green
 *  18    PC3   R: aux red
 *  19    PA0   UDPI
 *  20    PA1   Vbat sense (voltage divider)
 *
 */

#define HWDEF_C  wurkkos/ts26/hwdef.c

// allow using aux LEDs as extra channel modes
#include "fsm/chan-rgbaux.h"

// channel modes:
// * 0. main LEDs
// * 1+. aux RGB
#define NUM_CHANNEL_MODES   (1 + NUM_AUXRGB_CHANNEL_MODES)
enum channel_modes_e {
    CM_MAIN = 0,
    AUXRGB_CM_ENUMS
};

#define DEFAULT_CHANNEL_MODE  CM_MAIN

// right-most bit first, modes are in fedcba9876543210 order
#define CHANNEL_MODES_ENABLED 0b0000000000000001


#define PWM_BITS      16        // 0 to 32640 (0 to 255 PWM + 0 to 127 DSM) at constant kHz
#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // 15-bit PWM+DSM ramp

#define PWM_TOP  TCA0.SINGLE.PERBUF  // holds the TOP value for for variable-resolution PWM
#define PWM_TOP_INIT  255
#define PWM_CNT  TCA0.SINGLE.CNT  // for checking / resetting phase
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
#define DSM_TOP       (255<<7) // 15-bit resolution leaves 1 bit for carry

// timer interrupt for DSM
#define DSM_vect     TCA0_OVF_vect
#define DSM_INTCTRL  TCA0.SINGLE.INTCTRL
#define DSM_INTFLAGS TCA0.SINGLE.INTFLAGS
#define DSM_OVF_bm   TCA_SINGLE_OVF_bm

#define DELAY_FACTOR 90  // less time in delay() because more time spent in interrupts

// regulated channel
uint16_t ch1_dsm_lvl;
uint8_t ch1_pwm, ch1_dsm;
#define CH1_PIN  PB0
#define CH1_PWM  TCA0.SINGLE.CMP0BUF  // CMP0 is the output compare register for PB0

#define CH1_ENABLE_PIN   PIN2_bp
#define CH1_ENABLE_PORT  PORTB_OUT

// e-switch
#define SWITCH_PIN      PIN5_bp
#define SWITCH_PORT     VPORTA.IN
#define SWITCH_ISC_REG  PORTA.PIN5CTRL
#define SWITCH_VECT     PORTA_PORT_vect
#define SWITCH_INTFLG   VPORTA.INTFLAGS

// Voltage divider battLVL
#define USE_VOLTAGE_DIVIDER       // use a dedicated pin, not VCC, because VCC input is regulated
#define ADMUX_VOLTAGE_DIVIDER  ADC_MUXPOS_AIN1_gc  // which ADC channel to read
// don't use the default VDD converter
#undef voltage_raw2cooked
#define voltage_raw2cooked  mcu_vdivider_raw2cooked

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
//   (voltage - D1) * (R2/(R2+R1) * 4096 / 1.1)
// Resistors are 330k and 100k
#define ADC_44  4095  // raw value at 4.40V
#define ADC_22  2048  // raw value at 2.20V

// this light has RGB aux LEDs
#define USE_AUXRGB_LEDS

// aux RGB passive
#define AUXRGB_R_PORT  PORTC
#define AUXRGB_R_PIN   PIN3_bp
#define AUXRGB_G_PORT  PORTC
#define AUXRGB_G_PIN   PIN2_bp
#define AUXRGB_B_PORT  PORTC
#define AUXRGB_B_PIN   PIN1_bp


inline void hwdef_setup() {

    mcu_clock_speed();

    // Outputs:
    //VPORTA.DIR = ...;
    VPORTB.DIR = PIN0_bm   // PWM
               | PIN2_bm;  // boost enable
    VPORTC.DIR = PIN1_bm   // aux B
               | PIN2_bm   // aux G
               | PIN3_bm;  // aux R

    // enable pullups on the unused pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN1CTRL = PORT_PULLUPEN_bm;  // Vbat sense
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;  // eSwitch
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    //PORTB.PIN0CTRL = PORT_PULLUPEN_bm;  // regulated channel
    PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
    //PORTB.PIN2CTRL = PORT_PULLUPEN_bm;  // boost enable
    PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN5CTRL = PORT_PULLUPEN_bm;

    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
    //PORTC.PIN1CTRL = PORT_PULLUPEN_bm;  // RGB Aux
    //PORTC.PIN2CTRL = PORT_PULLUPEN_bm;  // RGB Aux
    //PORTC.PIN3CTRL = PORT_PULLUPEN_bm;  // RGB Aux

    // set up the PWM
    // https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf
    // PB0 is TCA0:WO0, use TCA_SINGLE_CMP0EN_bm
    // PB1 is TCA0:WO1, use TCA_SINGLE_CMP1EN_bm
    // PB2 is TCA0:WO2, use TCA_SINGLE_CMP2EN_bm
    // For Fast (Single Slope) PWM use TCA_SINGLE_WGMODE_SINGLESLOPE_gc
    // For Phase Correct (Dual Slope) PWM use TCA_SINGLE_WGMODE_DSBOTTOM_gc
    // See the manual for other pins, clocks, configs, portmux, etc
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm
                      //| TCA_SINGLE_CMP1EN_bm
                      | TCA_SINGLE_WGMODE_DSBOTTOM_gc;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
                      | TCA_SINGLE_ENABLE_bm;

    PWM_TOP = PWM_TOP_INIT;

    // set up interrupt for delta-sigma modulation
    // (moved to hwdef.c functions so it can be enabled/disabled based on ramp level)
    //DSM_INTCTRL |= DSM_OVF_bm;  // interrupt once for each timer cycle

}


#define LAYOUT_DEFINED

