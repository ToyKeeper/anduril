/*
 * config-mode.c: Config mode base functions for Anduril.
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

#ifndef CONFIG_MODE_C
#define CONFIG_MODE_C

#include "config-mode.h"

// general helper function for config modes
uint8_t number_entry_state(Event event, uint16_t arg);
// return value from number_entry_state()
volatile uint8_t number_entry_value;


// ask the user for a sequence of numbers, then save them and return to caller
uint8_t config_state_base(Event event, uint16_t arg,
                          uint8_t num_config_steps,
                          void (*savefunc)()) {
    static uint8_t config_step;
    if (event == EV_enter_state) {
        config_step = 0;
        set_level(0);
        return MISCHIEF_MANAGED;
    }
    // advance forward through config steps
    else if (event == EV_tick) {
        if (config_step < num_config_steps) {
            push_state(number_entry_state, config_step + 1);
        }
        else {
            // TODO: blink out some sort of success pattern
            savefunc();
            save_config();
            //set_state(retstate, retval);
            pop_state();
        }
        return MISCHIEF_MANAGED;
    }
    // an option was set (return from number_entry_state)
    else if (event == EV_reenter_state) {
        config_state_values[config_step] = number_entry_value;
        config_step ++;
        return MISCHIEF_MANAGED;
    }
    //return EVENT_NOT_HANDLED;
    // eat all other events; don't pass any through to parent
    return EVENT_HANDLED;
}

uint8_t number_entry_state(Event event, uint16_t arg) {
    static uint8_t value;
    static uint8_t blinks_left;
    static uint8_t entry_step;
    static uint16_t wait_ticks;
    if (event == EV_enter_state) {
        value = 0;
        blinks_left = arg;
        entry_step = 0;
        wait_ticks = 0;
        return MISCHIEF_MANAGED;
    }
    // advance through the process:
    // 0: wait a moment
    // 1: blink out the 'arg' value
    // 2: wait a moment
    // 3: "buzz" while counting clicks
    // 4: save and exit
    else if (event == EV_tick) {
        // wait a moment
        if ((entry_step == 0) || (entry_step == 2)) {
            if (wait_ticks < TICKS_PER_SECOND/2)
                wait_ticks ++;
            else {
                entry_step ++;
                wait_ticks = 0;
            }
        }
        // blink out the option number
        else if (entry_step == 1) {
            if (blinks_left) {
                if ((wait_ticks & 31) == 10) {
                    set_level(RAMP_SIZE/4);
                }
                else if ((wait_ticks & 31) == 20) {
                    set_level(0);
                }
                else if ((wait_ticks & 31) == 31) {
                    blinks_left --;
                }
                wait_ticks ++;
            }
            else {
                entry_step ++;
                wait_ticks = 0;
            }
        }
        else if (entry_step == 3) {  // buzz while waiting for a number to be entered
            wait_ticks ++;
            // buzz for N seconds after last event
            if ((wait_ticks & 3) == 0) {
                set_level(RAMP_SIZE/6);
            }
            else if ((wait_ticks & 3) == 2) {
                set_level(RAMP_SIZE/8);
            }
            // time out after 3 seconds
            if (wait_ticks > TICKS_PER_SECOND*3) {
                //number_entry_value = value;
                set_level(0);
                entry_step ++;
            }
        }
        else if (entry_step == 4) {
            number_entry_value = value;
            pop_state();
        }
        return MISCHIEF_MANAGED;
    }
    // count clicks
    else if (event == EV_click1_release) {
        empty_event_sequence();
        if (entry_step == 3) {  // only count during the "buzz"
            value ++;
            wait_ticks = 0;
            // flash briefly
            set_level(RAMP_SIZE/2);
        }
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}


#endif

