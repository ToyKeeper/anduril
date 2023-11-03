// config-mode.c: Config mode base functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/config-mode.h"

// general helper function for config modes
uint8_t number_entry_state(Event event, uint16_t arg);
// return value from number_entry_state()
volatile uint8_t number_entry_value;


#if defined(USE_CONFIG_COLORS) && (NUM_CHANNEL_MODES > 1)
// TODO: promote this to fsm-channels.c ?
void set_chan_if(bool cond, uint8_t chan) {
    if ((cond) && (chan != channel_mode))
        set_channel_mode(chan);
}
#endif

// allow the user to set a new value for a config option
// can be called two ways:
//   - with a "click" action: Configures first menu item only.
//   - with a "hold" action: Sets user select a menu item and then
//     choose a new value for it.  User should hold button until light
//     blinks N times, to choose menu item N.  Then let go, and light
//     should give a buzzing prompt to enter a number.  Click N times
//     at the prompt to set the new value to N.
// after completing this process, config state calls the savefunc callback
// and then returns to caller/parent state
uint8_t config_state_base(
        Event event,
        uint16_t arg,
        uint8_t num_config_steps,
        void (*savefunc)(uint8_t step, uint8_t value)) {

    static uint8_t config_step;
    #ifdef USE_CONFIG_COLORS
    static uint8_t orig_channel;
    #endif
    if (event == EV_enter_state) {
        #if defined(USE_CONFIG_COLORS) && (NUM_CHANNEL_MODES > 1)
        orig_channel = channel_mode;
        #endif
        config_step = 0;
        set_level(0);
        // if button isn't held, configure first menu item
        if (! button_last_state) {
            config_step ++;
            push_state(number_entry_state, 0);
        }
    }

    // if initial "hold" event still active
    // blink while holding to indicate option number
    #define B_CLICK_FLAGS      (B_CLICK|B_HOLD|B_PRESS|B_RELEASE|B_TIMEOUT)
    #define B_ANY_HOLD         (B_CLICK|B_HOLD|B_PRESS)
    #define B_ANY_HOLD_RELEASE (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT)
    else if ((event & B_CLICK_FLAGS) == B_ANY_HOLD) {
        if (config_step <= num_config_steps) {
            #if defined(USE_CONFIG_COLORS) && (NUM_CHANNEL_MODES > 1)
                uint8_t chan = config_step - 1;
                if (chan < NUM_CHANNEL_MODES)
                    set_chan_if(config_color_per_step, chan);
            #endif
            if ((TICKS_PER_SECOND/10) == (arg % (TICKS_PER_SECOND*3/2))) {
                config_step ++;
                // blink when config step advances
                if (config_step <= num_config_steps) {
                    #ifdef CONFIG_BLINK_CHANNEL
                    set_chan_if(!config_color_per_step, CONFIG_BLINK_CHANNEL);
                    #endif
                    set_level(RAMP_SIZE * 3 / 8);
                }
            }
            else {
                // stay on at a low level to indicate menu is active
                #ifdef CONFIG_WAITING_CHANNEL
                set_chan_if(!config_color_per_step, CONFIG_WAITING_CHANNEL);
                #endif
                set_level(RAMP_SIZE * 1 / 8);
            }
        } else {
            // turn off when end of menu is reached
            set_level(0);
        }
    }

    // button release: activate number entry for one menu item
    else if ((event & B_CLICK_FLAGS) == B_ANY_HOLD_RELEASE) {
        // ask the user for a number, if they selected a menu item
        if (config_step && config_step <= num_config_steps) {
            #if defined(USE_CONFIG_COLORS) && (NUM_CHANNEL_MODES > 1)
                // put the colors back how they were
                set_channel_mode(orig_channel);
            #endif
            push_state(number_entry_state, 0);
        }
        // exit after falling out of end of menu
        else {
            pop_state();
        }
    }

    // an option was set (return from number_entry_state)
    else if (event == EV_reenter_state) {
        // process value with parent's callback
        savefunc(config_step, number_entry_value);
        // make changes persist in eeprom
        save_config();
        pop_state();
    }

    #if defined(USE_CONFIG_COLORS) && (NUM_CHANNEL_MODES > 1)
    else if (event == EV_leave_state) {
        // put the colors back how they were
        set_channel_mode(orig_channel);
    }
    #endif

    // eat all other events; don't pass any through to parent
    return EVENT_HANDLED;
}

uint8_t number_entry_state(Event event, uint16_t arg) {
    static uint8_t entry_step;

    if (event == EV_enter_state) {
        number_entry_value = 0;
        entry_step = 0;
        set_level(0);  // initial pause should be dark
    }

    // advance through the process:
    // 0: wait a moment
    // 1: "buzz" while counting clicks
    // 2: save and exit
    else if (event == EV_tick) {
        // wait a moment
        if (entry_step == 0) {
            if (arg > TICKS_PER_SECOND/2) {
                entry_step ++;
                empty_event_sequence();  // reset tick counter to 0
            }
        }
        // buzz while waiting for a number to be entered
        else if (entry_step == 1) {
            // time out and exit after 3 seconds
            if (arg > TICKS_PER_SECOND*3) {
                entry_step ++;
                set_level(0);
            }
            // buzz for N seconds after last event
            // (flicker every other frame,
            //  except first frame (so we can see flashes after each click))
            else if (arg) {
                #ifdef CONFIG_WAITING_CHANNEL
                set_chan_if(1, CONFIG_WAITING_CHANNEL);
                #endif
                set_level( (RAMP_SIZE/8)
                           + ((arg&2)<<2) );
            }
        }
        // all done, save result and return to parent state
        else {
            pop_state();
        }
        return EVENT_HANDLED;
    }

    // count clicks: click = +1, hold = +10
    else if ((event == EV_click1_release)
            #ifdef USE_NUMBER_ENTRY_PLUS10
            || (event == EV_click1_hold_release)
            #endif
            ) {
        entry_step = 1;  // in case user clicked during initial delay
        #ifdef USE_NUMBER_ENTRY_PLUS10
            if (event == EV_click1_hold_release) number_entry_value += 10;
            else
        #endif
        number_entry_value ++;  // update the result
        empty_event_sequence();  // reset FSM's click count
        #ifdef CONFIG_BLINK_CHANNEL
        set_channel_mode(CONFIG_BLINK_CHANNEL);
        #endif
        set_level(RAMP_SIZE/2);  // flash briefly
        return EVENT_HANDLED;
    }

    // eat all other events; don't pass any through to parent
    return EVENT_HANDLED;
}

