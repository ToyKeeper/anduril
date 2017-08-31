#ifndef TK_ATTINY_H
#define TK_ATTINY_H
/*
 * Attiny portability header.
 * This helps abstract away the differences between various attiny MCUs.
 *
 * Copyright (C) 2017 Selene ToyKeeper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Choose your MCU here, or in the main .c file, or in the build script
//#define ATTINY 13
//#define ATTINY 25

/******************** hardware-specific values **************************/
#if (ATTINY == 13)
    #define F_CPU 4800000UL
    #define EEPSIZE 64
    #define V_REF REFS0
    #define BOGOMIPS 950
    #define ADMUX_VCC 0b00001100
#elif (ATTINY == 25)
    // TODO: Use 6.4 MHz instead of 8 MHz?
    #define F_CPU 8000000UL
    #define EEPSIZE 128
    #define V_REF REFS1
    #define BOGOMIPS (F_CPU/4000)
    #define ADMUX_VCC 0b00001100
    #define ADMUX_THERM 0b10001111
#elif (ATTINY == 85)
    // TODO: Use 6.4 MHz instead of 8 MHz?
    #define F_CPU 8000000UL
    #define EEPSIZE 512
    #define V_REF REFS1
    #define BOGOMIPS (F_CPU/4000)
    // (1 << V_REF) | (0 << ADLAR) | (VCC_CHANNEL)
    #define ADMUX_VCC 0b00001100
    // (1 << V_REF) | (0 << ADLAR) | (THERM_CHANNEL)
    #define ADMUX_THERM 0b10001111
#else
    #error Hey, you need to define ATTINY.
#endif


#include <avr/interrupt.h>

/******************** I/O pin and register layout ************************/
#ifdef FET_7135_LAYOUT
#define DRIVER_TYPE_DEFINED
/*
 *           ----
 *   Reset -|1  8|- VCC
 *     OTC -|2  7|- Voltage ADC
 *  Star 3 -|3  6|- PWM (FET)
 *     GND -|4  5|- PWM (1x7135)
 *           ----
 */

#define STAR2_PIN   PB0     // If this pin isn't used for ALT_PWM
#define STAR3_PIN   PB4     // pin 3

#define CAP_PIN     PB3     // pin 2, OTC
#define CAP_CHANNEL 0x03    // MUX 03 corresponds with PB3 (Star 4)
#define CAP_DIDR    ADC3D   // Digital input disable bit corresponding with PB3

#define PWM_PIN     PB1     // pin 6, FET PWM
#define PWM_LVL     OCR0B   // OCR0B is the output compare register for PB1
#define ALT_PWM_PIN PB0     // pin 5, 1x7135 PWM
#define ALT_PWM_LVL OCR0A   // OCR0A is the output compare register for PB0

#define VOLTAGE_PIN PB2     // pin 7, voltage ADC
#define ADC_CHANNEL 0x01    // MUX 01 corresponds with PB2
#define ADC_DIDR    ADC1D   // Digital input disable bit corresponding with PB2
#define ADC_PRSCL   0x06    // clk/64

//#define TEMP_DIDR   ADC4D
#define TEMP_CHANNEL 0b00001111

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels

#endif  // FET_7135_LAYOUT

#ifdef TRIPLEDOWN_LAYOUT
#define DRIVER_TYPE_DEFINED
/*
 *             ----
 *     Reset -|1  8|- VCC
 *       OTC -|2  7|- Voltage ADC
 * PWM (FET) -|3  6|- PWM (6x7135)
 *       GND -|4  5|- PWM (1x7135)
 *             ----
 */

#define STAR2_PIN   PB0     // If this pin isn't used for ALT_PWM

#define CAP_PIN     PB3     // pin 2, OTC
#define CAP_CHANNEL 0x03    // MUX 03 corresponds with PB3 (Star 4)
#define CAP_DIDR    ADC3D   // Digital input disable bit corresponding with PB3

#define PWM_PIN     PB1     // pin 6, 6x7135 PWM
#define PWM_LVL     OCR0B   // OCR0B is the output compare register for PB1
#define ALT_PWM_PIN PB0     // pin 5, 1x7135 PWM
#define ALT_PWM_LVL OCR0A   // OCR0A is the output compare register for PB0
#define FET_PWM_PIN PB4     // pin 3
#define FET_PWM_LVL OCR1B   // output compare register for PB4

#define VOLTAGE_PIN PB2     // pin 7, voltage ADC
#define ADC_CHANNEL 0x01    // MUX 01 corresponds with PB2
#define ADC_DIDR    ADC1D   // Digital input disable bit corresponding with PB2
#define ADC_PRSCL   0x06    // clk/64

//#define TEMP_DIDR   ADC4D
#define TEMP_CHANNEL 0b00001111

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels

#endif  // TRIPLEDOWN_LAYOUT

#ifdef FERRERO_ROCHER_LAYOUT
#define DRIVER_TYPE_DEFINED
/*
 *            ----
 *    Reset -|1  8|- VCC
 * E-switch -|2  7|- Voltage ADC
 *  Red LED -|3  6|- PWM
 *      GND -|4  5|- Green LED
 *            ----
 */
// TODO: fill in this section, update Ferrero_Rocher code to use it.
#define FAST 0x23           // fast PWM channel 1 only
#define PHASE 0x21          // phase-correct PWM channel 1 only
#endif  // FERRERO_ROCHER_LAYOUT

#ifdef NANJG_LAYOUT
#define DRIVER_TYPE_DEFINED
#define STAR2_PIN   PB0
#define STAR3_PIN   PB4
#define STAR4_PIN   PB3
#define PWM_PIN     PB1
#define VOLTAGE_PIN PB2
#define ADC_CHANNEL 0x01    // MUX 01 corresponds with PB2
#define ADC_DIDR    ADC1D   // Digital input disable bit corresponding with PB2
#define ADC_PRSCL   0x06    // clk/64

#define PWM_LVL     OCR0B   // OCR0B is the output compare register for PB1

#define FAST 0x23           // fast PWM channel 1 only
#define PHASE 0x21          // phase-correct PWM channel 1 only

#endif  // NANJG_LAYOUT


#ifdef FSM_EMISAR_D4_DRIVER
#define DRIVER_TYPE_DEFINED
/*
 *           ----
 *   Reset -|1  8|- VCC
 * eswitch -|2  7|-
 * AUX LED -|3  6|- PWM (FET)
 *     GND -|4  5|- PWM (1x7135)
 *           ----
 */

#define PWM_CHANNELS 2

#define AUXLED_PIN   PB4    // pin 3

#define SWITCH_PIN   PB3    // pin 2
#define SWITCH_PCINT PCINT3 // pin 2 pin change interrupt

#define PWM1_PIN PB0        // pin 5, 1x7135 PWM
#define PWM1_LVL OCR0A      // OCR0A is the output compare register for PB0
#define PWM2_PIN PB1        // pin 6, FET PWM
#define PWM2_LVL OCR0B      // OCR0B is the output compare register for PB1

#define VOLTAGE_PIN PB2     // pin 7, voltage ADC
#define ADC_CHANNEL 0x01    // MUX 01 corresponds with PB2
#define ADC_DIDR    ADC1D   // Digital input disable bit corresponding with PB2
#define ADC_PRSCL   0x06    // clk/64

//#define TEMP_DIDR   ADC4D
#define TEMP_CHANNEL 0b00001111

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels

#endif  // ifdef FSM_EMISAR_D4_DRIVER


#ifdef FSM_TKSABER_DRIVER
#define DRIVER_TYPE_DEFINED
/*
 *             ----
 *     Reset -|1  8|- VCC
 * PWM 4 (A) -|2  7|- e-switch
 * PWM 3 (B) -|3  6|- PWM 2 (G)
 *       GND -|4  5|- PWM 1 (R)
 *             ----
 */

#define PWM_CHANNELS 4
#define PWM1_PIN PB0        // pin 5
#define PWM1_LVL OCR0A
#define PWM2_PIN PB1        // pin 6
#define PWM2_LVL OCR0B
#define PWM3_PIN PB4        // pin 3
#define PWM3_LVL OCR1B
#define PWM4_PIN PB3        // pin 2
#define PWM4_LVL OCR1A      // FIXME: does this work?

#define SWITCH_PIN PB2      // pin 7
#define SWITCH_PCINT PCINT2 // pin 7 pin change interrupt

#define ADC_PRSCL   0x07    // clk/128 (no need to be super fast)
// FIXME: What is the DIDR for pin 8?
//#define ADC_DIDR    ADC1D   // Digital input disable bit corresponding with PB2

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels

#endif  // TKSABER_DRIVER


#ifndef DRIVER_TYPE_DEFINED
#error Hey, you need to define an I/O pin layout.
#endif

#endif  // TK_ATTINY_H
