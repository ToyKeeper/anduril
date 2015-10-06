#ifndef TK_ATTINY_H
#define TK_ATTINY_H
/*
 * Attiny portability header.
 * This helps abstract away the differences between various attiny MCUs.
 *
 * Copyright (C) 2015 Selene ToyKeeper
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
#elif (ATTINY == 25)
    // TODO: Use 6.4 MHz instead of 8 MHz?
    #define F_CPU 8000000UL
    #define EEPSIZE 128
    #define V_REF REFS1
    #define BOGOMIPS 2000
#else
    Hey, you need to define ATTINY.
#endif


/******************** I/O pin and register layout ************************/
#ifdef FET_7135_LAYOUT
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

#endif  // FET_7135_LAYOUT

#ifdef FERRERO_ROCHER_LAYOUT
/*
 *            ----
 *    Reset -|1  8|- VCC
 * E-switch -|2  7|- Voltage ADC
 *  Red LED -|3  6|- PWM
 *      GND -|4  5|- Green LED
 *            ----
 */
// TODO: fill in this section, update Ferrero_Rocher code to use it.
#endif  // FERRERO_ROCHER_LAYOUT

#ifdef NANJG_LAYOUT
#define STAR2_PIN   PB0
#define STAR3_PIN   PB4
#define STAR4_PIN   PB3
#define PWM_PIN     PB1
#define VOLTAGE_PIN PB2
#define ADC_CHANNEL 0x01    // MUX 01 corresponds with PB2
#define ADC_DIDR    ADC1D   // Digital input disable bit corresponding with PB2
#define ADC_PRSCL   0x06    // clk/64

#define PWM_LVL     OCR0B   // OCR0B is the output compare register for PB1
#endif  // NANJG_LAYOUT

#ifndef PWM_LVL
    Hey, you need to define an I/O pin layout.
#endif

#endif  // TK_ATTINY_H
