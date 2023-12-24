// tactical-mode.c: Tactical (ish) mode for Anduril.
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/tactical-mode.h"


uint8_t tactical_state(Event event, uint16_t arg) {
    // momentary(ish) tactical mode
    uint8_t ret = EVENT_NOT_HANDLED;

    // button is being held
    if ((event & (B_CLICK | B_PRESS)) == (B_CLICK | B_PRESS)) {
        // 1H: 1st level
        // 2H: 2nd level
        // 3H: 3rd level
        // 4+: nothing
        momentary_active = 0;
        ret = EVENT_HANDLED;
        uint8_t click = event & 0x0f; // click number
        if (click <= 3) {
            momentary_active = 1;
            uint8_t lvl = cfg.tactical_levels[click-1];
            if (lvl <= RAMP_SIZE) {  // steady output
                momentary_mode = 0;
                #if NUM_CHANNEL_MODES > 1
                    // use ramp mode's channel
                    channel_mode = cfg.channel_mode;
                #endif
                #ifdef USE_TACTICAL_MODE_SMOOTH_STEPS
                if (cfg.tactical_smooth_steps) off_state_set_level(lvl);
                else set_level(lvl);
                #else
                set_level(lvl);
                #endif
            } else {  // momentary strobe mode
                momentary_mode = 1;
                current_strobe_type = (lvl - RAMP_SIZE - 1) % strobe_mode_END;
            }
        }
    }
    // button was released
    else if ((event & (B_CLICK | B_PRESS)) == (B_CLICK)) {
        momentary_active = 0;
        if (momentary_mode) {
            set_level(0);
            interrupt_nice_delays();  // stop animations in progress
        } else {
            #ifdef USE_TACTICAL_MODE_SMOOTH_STEPS
            if (cfg.tactical_smooth_steps) off_state_set_level(0);
            else set_level(lvl);
            #else
            set_level(0);
            #endif
        }
    }

    // delegate to momentary mode while button is pressed
    if (momentary_active && momentary_mode) {
        momentary_state(event, arg);
    }

    // copy lockout mode's aux LED and sleep behaviors
    if (event == EV_enter_state) {
        lockout_state(event, arg);
    }
    else if (event == EV_tick) {
        if (! momentary_active) {
            return lockout_state(event, arg);
        }
        return EVENT_HANDLED;
    }
    else if (event == EV_sleep_tick) {
        return lockout_state(event, arg);
    }

    // 6 clicks: exit and turn off
    else if (event == EV_6clicks) {
        blink_once();
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }

    ////////// Every action below here is blocked in the simple UI //////////
    // (unnecessary since this entire mode is blocked in simple UI)
    /*
    #ifdef USE_SIMPLE_UI
    if (cfg.simple_ui_active) {
        return EVENT_NOT_HANDLED;
    }
    #endif
    */

    // 7H: configure tactical mode
    else if (event == EV_click7_hold) {
        push_state(tactical_config_state, 0);
        return EVENT_HANDLED;
    }

    return ret;
}

void tactical_config_save(uint8_t step, uint8_t value) {
    // update tac mode values
    // 3 values
    // each value is 1 to 150, or other:
    // - 1..150 is a ramp level
    // - other means "strobe mode"
    if (step <= 3){
      cfg.tactical_levels[step - 1] = value;
    }
    #ifdef USE_TACTICAL_MODE_SMOOTH_STEPS
    else if (step == 4){
      cfg.tactical_smooth_steps = value;
    }
    #endif
}

uint8_t tactical_config_state(Event event, uint16_t arg) {
    #ifdef USE_TACTICAL_MODE_SMOOTH_STEPS
    return config_state_base(event, arg, 3, tactical_config_save);
    #else
    return config_state_base(event, arg, 4, tactical_config_save);
    #endif
}

