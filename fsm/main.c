// fsm-main.c: main() function for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "fsm/main.h"


int main() {
    // Don't allow interrupts while booting
    cli();

    // prevents cycling after a crash,
    // whether intentional (like factory reset) or not (bugs)
    prevent_reboot_loop();

    hwdef_setup();

    #if 0
    #ifdef HALFSPEED
    // run at half speed
    // FIXME: not portable (also not needed)
    CLKPR = 1<<CLKPCE;
    CLKPR = 1;
    #endif
    #endif

    #ifdef USE_DEBUG_BLINK
    //debug_blink(1);
    #endif

    // all booted -- turn interrupts back on
    PCINT_on();
    // FIXME: support both WDT *and* RTC PIT on newer devices
    // (WDT to recover from crashes, PIT for task scheduling)
    // (old AVR had only WDT, newer ones split it into WDT, RTC, and PIT)
    WDT_on();
    ADC_on();
    sei();

    // in case any spurious button presses were detected at boot
    #ifdef USE_DELAY_MS
    delay_ms(1);
    #else
    delay_4ms(1);
    #endif

    // fallback for handling a few things
    #ifndef DONT_USE_DEFAULT_STATE
    push_state(default_state, 0);
    nice_delay_interrupt = 0;
    #endif

    // call recipe's setup
    setup();

    // main loop
    while (1) {
        // if event queue not empty, empty it
        process_emissions();

        // if loop() tried to change state, process that now
        StatePtr df = deferred_state;
        if (df) {
            set_state(df, deferred_state_arg);
            deferred_state = NULL;
            //deferred_state_arg = 0;  // unnecessary
        }

        // enter standby mode if requested
        // (works better if deferred like this)
        if (go_to_standby) {
            set_level(0);
            standby_mode();
        }

        // catch up on interrupts
        handle_deferred_interrupts();

        // turn delays back on, if they were off
        nice_delay_interrupt = 0;

        // give the recipe some time slices
        loop();

    }
}


void handle_deferred_interrupts() {
    /*
    if (irq_pcint) {  // button pressed or released
        // nothing to do here
        // (PCINT only matters during standby)
    }
    */
    if (irq_adc) {  // ADC done measuring
        adc_deferred();
        // irq_adc = 0;  // takes care of itself
    }
    if (irq_wdt) {  // the clock ticked
        WDT_inner();
        // irq_wdt = 0;  // takes care of itself
    }
}

