// Noctigon KR4 boost driver layout (attiny1634)
// Copyright (C) 2020-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/*
 * (based on Noctigon DM11-boost and KR4)
 * (is basically the same except the switch is on a different pin)
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
 *  17    PB2   MISO / e-switch (PCINT10)
 *  18    PB1   MOSI / battery voltage (ADC6)
 *  19    PB0   Opamp power
 *  20    PA7   (none)  (PCINT7)
 *      ADC12   thermal sensor
 *
 * Main LED power uses one pin to turn the Opamp on/off,
 * and one pin to control Opamp power level.
 * Linear brightness control uses the power level pin, with dynamic PWM.
 * The on/off pin is only used to turn the main LED on and off,
 * not to change brightness.
 */

#include "hank/noctigon-dm11/boost/hwdef.h"

// e-switch is on a different pin
#undef SWITCH_PIN
#undef SWITCH_PCINT
#undef SWITCH_PCIE
#undef SWITCH_PCMSK
#undef SWITCH_PORT
#undef SWITCH_PUE
#undef SWITCH_VECT
#define SWITCH_PIN   PB2     // pin 17
#define SWITCH_PCINT PCINT10 // pin 17 pin change interrupt
#define SWITCH_PCIE  PCIE1   // PCIE1 is for PCINT[11:8]
#define SWITCH_PCMSK PCMSK1  // PCMSK1 is for PCINT[11:8]
#define SWITCH_PORT  PINB    // PINA or PINB or PINC
#define SWITCH_PUE   PUEB    // pullup group B
#define SWITCH_VECT  PCINT1_vect  // ISR for PCINT[11:8]

