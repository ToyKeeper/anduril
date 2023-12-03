// fsm-pcint.h: PCINT (Pin Change Interrupt) functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

volatile uint8_t irq_pcint = 0;  // pin change interrupt happened?
//static volatile uint8_t button_was_pressed;
#define BP_SAMPLES 32
volatile uint8_t button_last_state;
uint8_t button_is_pressed();
//inline void PCINT_on();
//inline void PCINT_off();
#define PCINT_on   mcu_pcint_on
#define PCINT_off  mcu_pcint_off
void PCINT_inner(uint8_t pressed);

