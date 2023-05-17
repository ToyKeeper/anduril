// Noctigon DM11 boost driver layout (attiny1634)
// Copyright (C) 2021-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * (based on Noctigon K1)
 *
 * Pin / Name / Function
 *   1    PA6   (none) (PWM1B) (reserved for DD drivers)
 *   2    PA5   R: red aux LED (PWM0B)
 *   3    PA4   G: green aux LED
 *   4    PA3   B: blue aux LED
 *   5    PA2   L: button LED
 *   6    PA1   (none)
 *   7    PA0   (none)
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   (none)
 *  11    PC4   (none)
 *  12    PC3   RESET
 *  13    PC2   (none)
 *  14    PC1   SCK
 *  15    PC0   boost PMIC enable (PWM0A not used)
 *  16    PB3   main LED PWM (PWM1A)
 *  17    PB2   MISO (PCINT10)
 *  18    PB1   MOSI / battery voltage (ADC6)
 *  19    PB0   Opamp power
 *  20    PA7   e-switch  (PCINT7)
 *      ADC12   thermal sensor
 *
 * Main LED power uses one pin to turn the Opamp on/off,
 * and one pin to control Opamp power level.
 * Linear brightness control uses the power level pin, with dynamic PWM.
 * The on/off pin is only used to turn the main LED on and off,
 * not to change brightness.
 */

#define ATTINY 1634
#include <avr/io.h>

#define HWDEF_C_FILE hwdef-noctigon-dm11-boost.c

// allow using aux LEDs as extra channel modes
#include "chan-rgbaux.h"

#define USE_CHANNEL_MODES
// channel modes:
// * 0. main LEDs
// * 1. aux red
// * 2. aux yellow
// * 3. aux green
// * 4. aux cyan
// * 5. aux blue
// * 6. aux purple
// * 7. aux white
#define NUM_CHANNEL_MODES  8
enum CHANNEL_MODES {
    CM_MAIN = 0,
    CM_AUXRED,
    CM_AUXYEL,
    CM_AUXGRN,
    CM_AUXCYN,
    CM_AUXBLU,
    CM_AUXPRP,
    CM_AUXWHT,
};

#define DEFAULT_CHANNEL_MODE  CM_MAIN

#define CHANNEL_MODES_ENABLED 0b00000001
#define CHANNEL_HAS_ARGS      0b00000000
// no args
//#define USE_CHANNEL_MODE_ARGS
//#define CHANNEL_MODE_ARGS     0,0,0,0,0,0,0,0

#define SET_LEVEL_MODES      set_level_main, \
                             set_level_auxred, \
                             set_level_auxyel, \
                             set_level_auxgrn, \
                             set_level_auxcyn, \
                             set_level_auxblu, \
                             set_level_auxprp, \
                             set_level_auxwht
// gradual ticking for thermal regulation
#define GRADUAL_TICK_MODES   gradual_tick_main, \
                             gradual_tick_null, \
                             gradual_tick_null, \
                             gradual_tick_null, \
                             gradual_tick_null, \
                             gradual_tick_null, \
                             gradual_tick_null, \
                             gradual_tick_null


#define PWM_CHANNELS 1  // old, remove this

#define PWM_BITS      16        // dynamic 16-bit, but never goes over 255
#define PWM_GET       PWM_GET8
#define PWM_DATATYPE  uint16_t  // is used for PWM_TOPS (which goes way over 255)
#define PWM_DATATYPE2 uint16_t  // only needs 32-bit if ramp values go over 255
#define PWM1_DATATYPE uint8_t   // 1x7135 ramp

#define PWM_TOP       ICR1   // holds the TOP value for for variable-resolution PWM
#define PWM_TOP_INIT  255    // highest value used in top half of ramp
#define PWM_CNT       TCNT1  // for dynamic PWM, reset phase

#define CH1_PIN PB3            // pin 16, Opamp reference
#define CH1_PWM OCR1A          // OCR1A is the output compare register for PB3

#define PWM1_PHASE_RESET_OFF  // force reset while shutting off
#define PWM1_PHASE_RESET_ON   // force reset while turning on
#define PWM1_PHASE_SYNC       // manual sync while changing level

#define LED_ENABLE_PIN  PB0    // pin 19, Opamp power
#define LED_ENABLE_PORT PORTB  // control port for PB0

#define LED_ENABLE_PIN2  PC0   // pin 15, boost PMIC enable
#define LED_ENABLE_PORT2 PORTC // control port for PC0

// e-switch
#define SWITCH_PIN   PA7    // pin 20
#define SWITCH_PCINT PCINT7 // pin 20 pin change interrupt
#define SWITCH_PCIE  PCIE0  // PCIE0 is for PCINT[7:0]
#define SWITCH_PCMSK PCMSK0 // PCMSK0 is for PCINT[7:0]
#define SWITCH_PORT  PINA   // PINA or PINB or PINC
#define SWITCH_PUE   PUEA   // pullup group A
#define PCINT_vect   PCINT0_vect  // ISR for PCINT[7:0]

#define USE_VOLTAGE_DIVIDER  // use a dedicated pin, not VCC, because VCC input is flattened
#define VOLTAGE_PIN PB1      // Pin 18 / PB1 / ADC6
// pin to ADC mappings are in DS table 19-4
#define VOLTAGE_ADC      ADC6D  // digital input disable pin for PB1
// DIDR0/DIDR1 mappings are in DS section 19.13.5, 19.13.6
#define VOLTAGE_ADC_DIDR DIDR1  // DIDR channel for ADC6D
// DS tables 19-3, 19-4
// Bit   7     6     5      4     3    2    1    0
//     REFS1 REFS0 REFEN ADC0EN MUX3 MUX2 MUX1 MUX0
// MUX[3:0] = 0, 1, 1, 0 for ADC6 / PB1
// divided by ...
// REFS[1:0] = 1, 0 for internal 1.1V reference
// other bits reserved
#define ADMUX_VOLTAGE_DIVIDER 0b10000110
#define ADC_PRSCL   0x07    // clk/128

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
//   (voltage - D1) * (R2/(R2+R1) * 1024 / 1.1)
// D1, R1, R2 = 0, 330, 100
#ifndef ADC_44
//#define ADC_44 981  // raw value at 4.40V
#define ADC_44 967  // manually tweaked so 4.16V will blink out 4.2
#endif
#ifndef ADC_22
//#define ADC_22 489  // raw value at 2.20V
#define ADC_22 482  // manually tweaked so 4.16V will blink out 4.2
#endif

// this light has aux LEDs under the optic
#define AUXLED_R_PIN    PA5    // pin 2
#define AUXLED_G_PIN    PA4    // pin 3
#define AUXLED_B_PIN    PA3    // pin 4
#define AUXLED_RGB_PORT PORTA  // PORTA or PORTB or PORTC
#define AUXLED_RGB_DDR  DDRA   // DDRA or DDRB or DDRC
#define AUXLED_RGB_PUE  PUEA   // PUEA or PUEB or PUEC

#define BUTTON_LED_PIN  PA2    // pin 5
#define BUTTON_LED_PORT PORTA  // for all "PA" pins
#define BUTTON_LED_DDR  DDRA   // for all "PA" pins
#define BUTTON_LED_PUE  PUEA   // for all "PA" pins

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// it also has an independent LED in the button
#define USE_BUTTON_LED
// the aux LEDs are front-facing, so turn them off while main LEDs are on
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

void set_level_main(uint8_t level);

bool gradual_tick_main(uint8_t gt);


inline void hwdef_setup() {
    // enable output ports
    // boost PMIC on/off
    DDRC = (1 << LED_ENABLE_PIN2);
    // Opamp level and Opamp on/off
    DDRB = (1 << CH1_PIN)
         | (1 << LED_ENABLE_PIN);
    // aux R/G/B, button LED
    DDRA = (1 << AUXLED_R_PIN)
         | (1 << AUXLED_G_PIN)
         | (1 << AUXLED_B_PIN)
         | (1 << BUTTON_LED_PIN)
         ;

    // configure PWM
    // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
    // pre-scale for timer: N = 1
    // WGM1[3:0]: 1,0,1,0: PWM, Phase Correct, adjustable (DS table 12-5)
    // CS1[2:0]:    0,0,1: clk/1 (No prescaling) (DS table 12-6)
    // COM1A[1:0]:    1,0: PWM OC1A in the normal direction (DS table 12-4)
    // COM1B[1:0]:    0,0: PWM OC1B disabled (DS table 12-4)
    TCCR1A = (1<<WGM11)  | (0<<WGM10)   // adjustable PWM (TOP=ICR1) (DS table 12-5)
           | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A in normal direction (DS table 12-4)
           | (0<<COM1B1) | (0<<COM1B0)  // PWM 1B disabled (DS table 12-4)
           ;
    TCCR1B = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
           | (1<<WGM13)  | (0<<WGM12)  // phase-correct adjustable PWM (DS table 12-5)
           ;

    // set PWM resolution
    PWM_TOP = PWM_TOP_INIT;

    // set up e-switch
    SWITCH_PUE = (1 << SWITCH_PIN);  // pull-up for e-switch
    SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}


#define LAYOUT_DEFINED

