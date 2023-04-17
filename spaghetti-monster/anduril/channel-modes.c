// channel-modes.c: Multi-channel functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "channel-modes.h"

uint8_t channel_mode_state(Event event, uint16_t arg) {
    #ifdef USE_CHANNEL_MODE_ARGS
    static int8_t tint_ramp_direction = 1;
    static uint8_t prev_tint = 0;
    // don't activate auto-tint modes unless the user hits the edge
    // and keeps pressing for a while
    static uint8_t past_edge_counter = 0;
    // bugfix: click-click-hold from off to strobes would invoke tint ramping
    // in addition to changing state...  so ignore any tint-ramp events which
    // don't look like they were meant to be here
    static uint8_t active = 0;
    uint8_t tint = cfg.channel_mode_args[cfg.channel_mode];
    #endif

    // it's possible that a light may need 3H but not 3C,
    // so try to detect if 3C is needed
    #if NUM_CHANNEL_MODES > 1
    // 3 clicks: next channel mode
    if (event == EV_3clicks) {
        uint8_t next = cfg.channel_mode;
        // go to next channel mode until we find one which is enabled
        // (and don't do any infinite loops if the user disabled them all)
        uint8_t count = 0;
        do {
            count ++;
            next = (next + 1) % NUM_CHANNEL_MODES;
        } while ((! channel_mode_enabled(next)) && count < NUM_CHANNEL_MODES);
        //} while ((! channel_modes_enabled[next]) && count < NUM_CHANNEL_MODES);

        // undo change if infinite loop detected (redundant?)
        //if (NUM_CHANNEL_MODES == count) next = cfg.channel_mode;

        // if mode hasn't changed, abort
        if (cfg.channel_mode == next)
            return EVENT_NOT_HANDLED;

        set_channel_mode(next);

        // remember after battery changes
        save_config();
        return EVENT_HANDLED;
    } else
    #endif  // if NUM_CHANNEL_MODES > 1

    #ifdef USE_CUSTOM_CHANNEL_3H_MODES
    // defer to mode-specific function if defined
    if (tint_ramp_modes[cfg.channel_mode]) {
        StatePtr tint_func = channel_3H_modes[cfg.channel_mode];
        return tint_func(cfg.channel_mode);
    } else
    #endif
    #ifdef USE_CHANNEL_MODE_ARGS
    #ifndef DONT_USE_DEFAULT_CHANNEL_ARG_MODE
    // click, click, hold: change the current channel's arg (like tint)
    if (event == EV_click3_hold) {
        ///// adjust value from 0 to 255
        // reset at beginning of movement
        if (! arg) {
            active = 1;  // first frame means this is for us
            past_edge_counter = 0;  // doesn't start until user hits the edge
        }
        // ignore event if we weren't the ones who handled the first frame
        if (! active) return EVENT_NOT_HANDLED;

        // change normal tints
        if ((tint_ramp_direction > 0) && (tint < 255)) {
            tint += 1;
        }
        else if ((tint_ramp_direction < 0) && (tint > 0)) {
            tint -= 1;
        }
        // if tint change stalled, let user know we hit the edge
        else if (prev_tint == tint) {
            if (past_edge_counter == 0) blip();
            past_edge_counter = 1;
        }
        prev_tint = tint;
        cfg.channel_mode_args[cfg.channel_mode] = tint;
        set_level(actual_level);
        return EVENT_HANDLED;
    }

    // click, click, hold, release: reverse direction for next ramp
    else if (event == EV_click3_hold_release) {
        active = 0;  // ignore next hold if it wasn't meant for us
        // reverse
        tint_ramp_direction = -tint_ramp_direction;
        if (0 == tint) tint_ramp_direction = 1;
        else if (255 == tint) tint_ramp_direction = -1;
        // remember tint after battery change
        cfg.channel_mode_args[cfg.channel_mode] = tint;
        save_config();
        // bug?: for some reason, brightness can seemingly change
        // from 1/150 to 2/150 without this next line... not sure why
        set_level(actual_level);
        return EVENT_HANDLED;
    }
    #endif  // ifndef DONT_USE_DEFAULT_CHANNEL_ARG_MODE
    #endif  // ifdef USE_CHANNEL_MODE_ARGS

    #if defined(USE_SIMPLE_UI)
    // remaining mappings aren't "simple", so stop here
    if (cfg.simple_ui_active) {
        return EVENT_NOT_HANDLED;
    }
    #endif

    #if NUM_CHANNEL_MODES > 1
    // channel toggle menu on ... 9H?
    else if (event == EV_click9_hold) {
        push_state(channel_mode_config_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif

    return EVENT_NOT_HANDLED;
}

#if NUM_CHANNEL_MODES > 1
void channel_mode_config_save(uint8_t step, uint8_t value) {
    // 1 menu item per channel mode, to enable or disable that mode
    step --;  // step is 1-based, channel modes are 0-based
    if (value) channel_mode_enable(step);
    else channel_mode_disable(step);
}

uint8_t channel_mode_config_state(Event event, uint16_t arg) {
    // 1 menu item per channel mode, to enable or disable that mode
    return config_state_base(
        event, arg,
        NUM_CHANNEL_MODES,
        channel_mode_config_save
        );
}
#endif

