// Noctigon KR4 w/ tint ramping
// Copyright (C) 2021-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * (same driver as emisar-2ch, but with the switch on a different pin)
 *
 * Pin / Name / Function
 *   1    PA6   2nd LED PWM (linear) (PWM1B)
 *   2    PA5   R: red aux LED (PWM0B)
 *   3    PA4   G: green aux LED
 *   4    PA3   B: blue aux LED
 *   5    PA2   button LED
 *   6    PA1   Opamp 2 enable (2nd LEDs)
 *   7    PA0   Opamp 1 enable (main LEDs)
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   (none)
 *  11    PC4   (none)
 *  12    PC3   RESET
 *  13    PC2   (none)
 *  14    PC1   SCK
 *  15    PC0   main LED PWM (FET) (PWM0A) (unused on some models because tint ramping)
 *  16    PB3   main LED PWM (linear) (PWM1A)
 *  17    PB2   MISO / e-switch (PCINT10)
 *  18    PB1   MOSI / battery voltage (ADC6)
 *  19    PB0   (none)
 *  20    PA7   (none)
 *      ADC12   thermal sensor
 */

// move the switch to a different pin
#define SWITCH_PIN   PB2     // pin 17
#define SWITCH_PCINT PCINT10 // pin 17 pin change interrupt
#define SWITCH_PCIE  PCIE1   // PCIE1 is for PCINT[11:8]
#define SWITCH_PCMSK PCMSK1  // PCMSK1 is for PCINT[11:8]
#define SWITCH_PORT  PINB    // PINA or PINB or PINC
#define SWITCH_PUE   PUEB    // pullup group B
#define SWITCH_VECT  PCINT1_vect  // ISR for PCINT[11:8]

// the rest of the config is the same as the generic Emisar 2ch build
#include "hank/emisar-2ch/hwdef.h"

