// lume1 Driver Rev B for FW3x driver layout (attiny1634)
// Copyright (C) 2020-2023 LoneOceans, Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/* lume1 Driver Rev B for FW3x driver layout (attiny1634)
 * www.loneoceans.com/labs/ for more information
 *
 * Pin / Name / Function in Lume1 Rev B
 *   1    PA6   Regulated PWM (PWM1B)
 *   2    PA5   R: red aux LED (PWM0B)
 *   3    PA4   G: green aux LED
 *   4    PA3   B: blue aux LED
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

#include <avr/io.h>

#define HWDEF_C_FILE lumintop/fw3x-lume1/hwdef.c

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


#define PWM_CHANNELS  2  // old, remove this

// Added for Lume1 Buck Boost Driver
#define PWM_BITS      16        // 0 to 1023 at 3.9 kHz, not 0 to 255 at 15.6 kHz
#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint16_t  // regulated ramp
#define PWM2_DATATYPE uint16_t  // DD FET ramp

// PWM parameters of both channels are tied together because they share a counter
#define PWM_TOP       ICR1   // holds the TOP value for variable-resolution PWM
#define PWM_TOP_INIT  1023
#define PWM_CNT       TCNT1  // for dynamic PWM, reset phase

// regulated channel
#define CH1_PIN  PA6            // pin 1, Buck Boost CTRL pin or 7135-eqv PWM
#define CH1_PWM  OCR1B          // OCR1B is the output compare register for PA6
#define CH1_ENABLE_PIN   PA7    // pin 20, BuckBoost Enable
#define CH1_ENABLE_PORT  PORTA  // control port for PA7

// DD FET channel
#define CH2_PIN  PB3            // pin 16, FET PWM Pin, but only used as on (1023) or off (0)
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
#define PCINT_vect   PCINT0_vect  // ISR for PCINT[7:0]

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

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
// [(Vbatt)*(R2/(R2+R1)) / 2.5V] * 1023
// R1 = R2 = 100kR
#ifndef ADC_44
#define ADC_44 900
#endif
#ifndef ADC_22
#define ADC_22 450
#endif

// Default ADMUX_THERM for Temperature is: 0b10001110 in arch/mcu.h
// REFS[1:0] as 10 for analog reference at internal 1.1Vref
// MUX[3:0] as 1110 for ADC'12' - temperature sensor internal

// Modified fsm-adc.c to use different ADMUX and ADC_temperature_handler()
// based on USE_EXTERNAL_TEMP_SENSOR
// See line 34 and line 209
#define USE_EXTERNAL_TEMP_SENSOR
#define ADMUX_THERM_EXTERNAL_SENSOR 0b00001011  // VCC reference (2.5V), Channel PC2
// Used for Lume1 Driver: MCP9700 - T_Celsius = 100*(VOUT - 0.5V)
// ADC is 2.5V reference, 0 to 1023
// FIXME: due to floating point, this calculation takes 916 extra bytes
//        (should use an integer equivalent instead)
#define EXTERN_TEMP_FORMULA(m) (((m)-205)/4.09)

// this driver allows for aux LEDs under the optic
#define AUXLED_R_PIN    PA5    // pin 2
#define AUXLED_G_PIN    PA4    // pin 3
#define AUXLED_B_PIN    PA3    // pin 4
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

    // configure PWM for 10 bit at 3.9kHz
    // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
    // pre-scale for timer: N = 1
    // WGM1[3:0]: 0,0,1,1: PWM, Phase Correct, 10-bit (DS table 12-5)
    // CS1[2:0]:    0,0,1: clk/1 (No prescaling) (DS table 12-6)
    // COM1A[1:0]:    1,0: PWM OC1A in the normal direction (DS table 12-4)
    // COM1B[1:0]:    1,0: PWM OC1B in the normal direction (DS table 12-4)
    TCCR1A  = (1<<WGM11)  | (1<<WGM10)   // 10-bit (TOP=0x03FF) (DS table 12-5)
            | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A Clear OC1A on Compare Match
            | (1<<COM1B1) | (0<<COM1B0)  // PWM 1B Clear OC1B on Compare Match
            ;
    TCCR1B  = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
            | (0<<WGM13)  | (0<<WGM12)  // PWM, Phase Correct, 10-bit
            ;

    // set PWM resolution
    PWM_TOP = PWM_TOP_INIT;

    // set up e-switch
    SWITCH_PUE = (1 << SWITCH_PIN);  // pull-up for e-switch
    SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}


#define LAYOUT_DEFINED

