/*
 * SpaghettiMonster: Generic foundation code for e-switch flashlights.
 * Other possible names:
 * - FSM
 * - RoundTable
 * - Mostly Harmless
 * - ...
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
 */

#include "tk-attiny.h"

#include <avr/eeprom.h>
#include <avr/power.h>

// include project definitions to help with recognizing symbols
#include "fsm-events.h"
#include "fsm-states.h"
#include "fsm-adc.h"
#include "fsm-wdt.h"
#include "fsm-pcint.h"
#include "fsm-standby.h"
#include "fsm-ramping.h"
#include "fsm-random.h"
#ifdef USE_EEPROM
#include "fsm-eeprom.h"
#endif
#include "fsm-misc.h"
#include "fsm-main.h"

#if defined(USE_DELAY_MS) || defined(USE_DELAY_4MS) || defined(USE_DELAY_ZERO) || defined(USE_DEBUG_BLINK)
#define OWN_DELAY
#include "tk-delay.h"
#endif

#ifdef USE_DEBUG_BLINK
#define DEBUG_FLASH PWM1_LVL = 64; delay_4ms(2); PWM1_LVL = 0;
void debug_blink(uint8_t num) {
    for(; num>0; num--) {
        PWM1_LVL = 32;
        delay_4ms(100/4);
        PWM1_LVL = 0;
        delay_4ms(100/4);
    }
}
#endif

// Define these in your SpaghettiMonster recipe
// boot-time tasks
void setup();
// single loop iteration, runs continuously
void loop();

// include executable functions too, for easier compiling
#include "fsm-states.c"
#include "fsm-events.c"
#include "fsm-adc.c"
#include "fsm-wdt.c"
#include "fsm-pcint.c"
#include "fsm-standby.c"
#include "fsm-ramping.c"
#include "fsm-random.c"
#ifdef USE_EEPROM
#include "fsm-eeprom.c"
#endif
#include "fsm-misc.c"
#include "fsm-main.c"
