// spaghetti-monster.h: UI toolkit / microkernel for e-switch flashlights.
// Copyright (C) 2017-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

/*
 * SpaghettiMonster: Generic foundation code for e-switch flashlights.
 * Other possible names:
 * - FSM
 * - RoundTable
 * - Mostly Harmless
 * - ...
 */

////////// include all the .h files //////////

#include "arch/mcu.h"

// include project definitions to help with recognizing symbols
#include "fsm/events.h"
#include "fsm/states.h"
#include "fsm/adc.h"
#include "fsm/wdt.h"
#include "fsm/pcint.h"
#include "fsm/standby.h"
#include "fsm/channels.h"
#include "fsm/ramping.h"
#include "fsm/random.h"
#ifdef USE_EEPROM
#include "fsm/eeprom.h"
#endif
#include "fsm/misc.h"
#include "fsm/main.h"

#if defined(USE_DELAY_MS) || defined(USE_DELAY_4MS) || defined(USE_DELAY_ZERO) || defined(USE_DEBUG_BLINK)
#define OWN_DELAY
#include "arch/delay.h"
#endif

#ifdef USE_DEBUG_BLINK
#define DEBUG_FLASH PWM1_LVL = 64; delay_4ms(2); PWM1_LVL = 0;
void debug_blink(uint8_t num);
#endif

// Define these in your SpaghettiMonster recipe
// boot-time tasks
uint8_t just_booted;
void setup();
// single loop iteration, runs continuously
void loop();

