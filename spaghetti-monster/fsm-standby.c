/*
 * fsm-standby.c: standby mode functions for SpaghettiMonster.
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

#ifndef FSM_STANDBY_C
#define FSM_STANDBY_C

#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "fsm-adc.h"
#include "fsm-wdt.h"
#include "fsm-pcint.h"

// low-power standby mode used while off but power still connected
#define standby_mode sleep_until_eswitch_pressed
void sleep_until_eswitch_pressed()
{
    #ifdef TICK_DURING_STANDBY
    WDT_slow();
    #else
    WDT_off();
    #endif

    ADC_off();

    // make sure switch isn't currently pressed
    while (button_is_pressed()) {}
    empty_event_sequence();  // cancel pending input on suspend

    PCINT_on();  // wake on e-switch event

    #ifdef TICK_DURING_STANDBY
    // detect which type of event caused a wake-up
    irq_adc = 0;
    irq_wdt = 0;
    irq_pcint = 0;
    while (go_to_standby) {
    #else
        go_to_standby = 0;
    #endif
        // configure sleep mode
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);

        sleep_enable();
        #ifdef BODCR  // only do this on MCUs which support it
        sleep_bod_disable();
        #endif
        sleep_cpu();  // wait here

        // something happened; wake up
        sleep_disable();

    #ifdef TICK_DURING_STANDBY
        // determine what woke us up...
        if (irq_pcint) {  // button pressed; wake up
            go_to_standby = 0;
        }
        if (irq_adc) {  // ADC done measuring
            adcint_enable = 1;
            ADC_inner();
            //ADC_off();  // takes care of itself
            //irq_adc = 0;  // takes care of itself
        }
        if (irq_wdt) {  // generate a sleep tick
            WDT_inner();
        }
    }
    #endif

    #ifdef USE_THERMAL_REGULATION
    // forget what the temperature was last time we were on
    reset_thermal_history = 1;
    #endif

    // go back to normal running mode
    //PCINT_on();  // should be on already
    ADC_on();
    WDT_on();
}

#ifdef USE_IDLE_MODE
void idle_mode()
{
    // configure sleep mode
    set_sleep_mode(SLEEP_MODE_IDLE);

    sleep_enable();
    sleep_cpu();  // wait here

    // something happened; wake up
    sleep_disable();
}
#endif

#endif
