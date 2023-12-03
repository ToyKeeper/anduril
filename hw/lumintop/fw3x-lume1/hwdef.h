// lume1 Driver Rev B for FW3x driver layout (attiny1634)
// Copyright (C) 2020-2023 LoneOceans, Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/* lume1 Driver Rev B for FW3x driver layout (attiny1634)
 * www.loneoceans.com/labs/ for more information
 *
 * Pin / Name / Function in Lume1 Rev B
 *   1    PA6   Regulated PWM (PWM1B)
 *   2    PA5   B: blue aux LED (PWM0B) (or red)
 *   3    PA4   G: green aux LED
 *   4    PA3   R: red aux LED  (or blue)
 *   5    PA2   e-switch (PCINT2)
 *   6    PA1   Jumper 1
 *   7    PA0   Jumper 2
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   Jumper 3
 *  11    PC4   Jumper 4
 *  12    PC3   RESET
 *  13    PC2   External Thermal Sensor (ADC11)
 *  14    PC1   SCK
 *  15    PC0   N/C
 *  16    PB3   FET PWM (PWM1A)
 *  17    PB2   MISO
 *  18    PB1   MOSI
 *  19    PB0   ADC5 Voltage Battery Sense (2:1 divider)
 *  20    PA7   BB_Enable
 *        ADC12 internal thermal sensor (not used for lume1)
 *
 * Main LED power uses one pin as a global Buck Boost Enable, and
 * one pin to control the power level via PWM.
 * Another pin is used for DD FET control.
 */

#define HWDEF_C  lumintop/fw3x-lume1/hwdef.c

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


#define PWM_BITS      16        // 0 to 32640 (0 to 255 PWM + 0 to 127 DSM) at constant kHz
#define PWM_DATATYPE  uint16_t
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // regulated ramp
#define PWM1_GET(x)   PWM_GET16(pwm1_levels, x)
#define PWM2_DATATYPE uint8_t   // DD FET ramp
#define PWM2_GET(x)   PWM_GET8(pwm2_levels, x)

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP       ICR1   // holds the TOP value for variable-resolution PWM
#define PWM_TOP_INIT  255
#define PWM_CNT       TCNT1  // for checking / resetting phase
// (max is (255 << 7), because it's 8-bit PWM plus 7 bits of DSM)
#define DSM_TOP       (255<<7) // 15-bit resolution leaves 1 bit for carry

// timer interrupt for DSM
#define DSM_vect     TIMER1_OVF_vect
#define DSM_INTCTRL  TIMSK
#define DSM_OVF_bm   (1<<TOIE1)

#define DELAY_FACTOR 85  // less time in delay() because more time spent in interrupts

// regulated channel uses PWM+DSM
uint16_t ch1_dsm_lvl;
uint8_t ch1_pwm, ch1_dsm;
#define CH1_PIN  PA6            // pin 1, Buck Boost CTRL pin or 7135-eqv PWM
#define CH1_PWM  OCR1B          // OCR1B is the output compare register for PA6
#define CH1_ENABLE_PIN   PA7    // pin 20, BuckBoost Enable
#define CH1_ENABLE_PORT  PORTA  // control port for PA7

// DD FET channel is on/off only (PWM=0, or PWM=MAX)
#define CH2_PIN  PB3            // pin 16, FET PWM Pin, but only used as on (255) or off (0)
#define CH2_PWM  OCR1A          // OCR1A is the output compare register for PB3

/* // For Jumpers X1 to X4, no SW support yet
#define JUMPER1_PIN PA1
#define JUMPER2_PIN PA0
#define JUMPER3_PIN PC5
#define JUMPER4_PIN PC4
*/

// e-switch
#define SWITCH_PIN   PA2    // pin 5
#define SWITCH_PCINT PCINT2 // pin 5 pin change interrupt
#define SWITCH_PCIE  PCIE0  // PCIE0 is for PCINT[7:0]
#define SWITCH_PCMSK PCMSK0 // PCMSK0 is for PCINT[7:0]
#define SWITCH_PORT  PINA   // PINA or PINB or PINC
#define SWITCH_PUE   PUEA   // pullup group A
#define SWITCH_VECT  PCINT0_vect  // ISR for PCINT[7:0]

#define USE_VOLTAGE_DIVIDER  // use a dedicated pin, not VCC, because VCC input is flattened
#define VOLTAGE_PIN PB0      // Pin 19  PB0  ADC5
// pin to ADC mappings are in DS table 19-4
#define VOLTAGE_ADC ADC5D  // digital input disable pin for PB1
// DIDR0/DIDR1 mappings are in DS section 19.13.5, 19.13.6
#define VOLTAGE_ADC_DIDR DIDR1  // DIDR channel for ADC5D
// DS tables 19-3, 19-4
// Bit   7     6     5      4     3    2    1    0
//     REFS1 REFS0 REFEN ADC0EN MUX3 MUX2 MUX1 MUX0
// MUX[3:0] = 0, 1, 0, 1 for ADC5 / PB0
// divided by ...
// REFS[1:0] = 0, 0 for VCC as analog reference at 2.5V
// other bits reserved
#define ADMUX_VOLTAGE_DIVIDER 0b00000101
#define ADC_PRSCL   0x06    // clk/64

#undef voltage_raw2cooked
#define voltage_raw2cooked  mcu_vdivider_raw2cooked

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
// [(Vbatt)*(R2/(R2+R1)) / 2.5V] * 1023
// R1 = R2 = 100kR
#ifndef ADC_44
#define ADC_44 (4*900)
#endif
#ifndef ADC_22
#define ADC_22 (4*450)
#endif

// Default ADMUX_THERM for Temperature is: 0b10001110 in arch/mcu.h
// REFS[1:0] as 10 for analog reference at internal 1.1Vref
// MUX[3:0] as 1110 for ADC'12' - temperature sensor internal

// Modified fsm-adc.c to use different ADMUX and ADC_temperature_handler()
// based on USE_EXTERNAL_TEMP_SENSOR
// See line 34 and line 209
//#define USE_EXTERNAL_TEMP_SENSOR

// override the default temperature sensor code
#undef hwdef_set_admux_therm
void hwdef_set_admux_therm();
#undef temp_raw2cooked
uint16_t temp_raw2cooked(uint16_t measurement);
// VCC reference (2.5V), Channel PC2
#define ADMUX_THERM_EXTERNAL_SENSOR 0b00001011

// this driver allows for aux LEDs under the optic
#ifdef FW3X_RGB_SWAP  // wiring fixed by end user
    #define AUXLED_R_PIN    PA5    // pin 2
    #define AUXLED_G_PIN    PA4    // pin 3
    #define AUXLED_B_PIN    PA3    // pin 4
#else  // Lumintop's factory wiring
    #define AUXLED_R_PIN    PA3    // pin 4
    #define AUXLED_G_PIN    PA4    // pin 3
    #define AUXLED_B_PIN    PA5    // pin 2
#endif
#define AUXLED_RGB_PORT PORTA  // PORTA or PORTB or PORTC
#define AUXLED_RGB_DDR  DDRA   // DDRA or DDRB or DDRC
#define AUXLED_RGB_PUE  PUEA   // PUEA or PUEB or PUEC

// For lume1 driver, no SW support for Auxillary Jumpers X1 to X4 yet!
inline void hwdef_setup() {
    // enable output ports
    // FET PWM Pin
    DDRB = (1 << CH2_PIN);
    // Main PWM, Buck Boost Enable Pin, aux R/G/B
    DDRA = (1 << CH1_PIN)
         | (1 << CH1_ENABLE_PIN)
         | (1 << AUXLED_R_PIN)
         | (1 << AUXLED_G_PIN)
         | (1 << AUXLED_B_PIN)
         ;

    //DDRB&=~(1<<VOLTAGE_PIN); // All pins are input by default
    /* // For Jumpers X1 to X4, no SW support yet
    DDRA &= (1<<JUMPER1_PIN);
    DDRA &= (1<<JUMPER2_PIN);
    DDRC &= (1<<JUMPER3_PIN);
    DDRC &= (1<<JUMPER4_PIN);
    PUEA = (1 << JUMPER1_PIN);
    PUEA = (1 << JUMPER2_PIN);
    PUEC = (1 << JUMPER3_PIN);
    PUEC = (1 << JUMPER4_PIN);
    */

    // configure PWM
    // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
    // pre-scale for timer: N = 1
    // PWM for both channels
    // WGM1[3:0]: 1,0,1,0: PWM, Phase Correct, adjustable (DS table 12-5)
    // WGM1[3:0]: 1,1,1,0: PWM, Fast, adjustable (DS table 12-5)
    // CS1[2:0]:    0,0,1: clk/1 (No prescaling) (DS table 12-6)
    // COM1A[1:0]:    1,0: PWM OC1A in the normal direction (DS table 12-4)
    // COM1B[1:0]:    1,0: PWM OC1B in the normal direction (DS table 12-4)
    TCCR1A = (1<<WGM11)  | (0<<WGM10)   // adjustable PWM (TOP=ICR1) (DS table 12-5)
           | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A in normal direction (DS table 12-4)
           | (1<<COM1B1) | (0<<COM1B0)  // PWM 1B in normal direction (DS table 12-4)
           ;
    TCCR1B = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
           //| (1<<WGM13)  | (1<<WGM12)  // fast adjustable PWM (DS table 12-5)
           | (1<<WGM13)  | (0<<WGM12)  // phase-correct adjustable PWM (DS table 12-5)
           ;

    // set PWM resolution
    PWM_TOP = PWM_TOP_INIT;

    // set up interrupt for delta-sigma modulation
    // (moved to hwdef.c functions so it can be enabled/disabled based on ramp level)
    //DSM_INTCTRL |= DSM_OVF_bm;  // interrupt once for each timer cycle

    // set up e-switch
    SWITCH_PUE = (1 << SWITCH_PIN);  // pull-up for e-switch
    SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}


#define LAYOUT_DEFINED

