// channel-modes.c: Multi-channel functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/channel-modes.h"

typedef struct RampState {
    int8_t tint_ramp_direction;
    uint8_t prev_tint;
    // Blip when the user hits the end of the ramp.
    uint8_t past_edge_counter;
} RampState;

uint8_t default_channel_ramp(uint16_t arg, RampState *state, uint8_t channel_args[]);
uint8_t default_channel_ramp_reverse(RampState *state, uint8_t channel_args[]);

uint8_t channel_mode_state(Event event, uint16_t arg) {
    #ifdef USE_CHANNEL_MODE_ARGS
    // bugfix: click-click-hold from off to strobes would invoke tint ramping
    // in addition to changing state...  so ignore any tint-ramp events which                                                                                                     -    // don't look like they were meant to be here 
    // don't look like they were meant to be here
    static uint8_t active = 0;
    static RampState ramp_state = {
        .tint_ramp_direction = 1,
        .prev_tint           = 0,
        .past_edge_counter   = 0,
    };
    #endif
    #ifdef USE_SECONDARY_CHANNEL_MODE_ARGS
    static RampState secondary_ramp_state = {
        .tint_ramp_direction = 1,
        .prev_tint           = 0,
        .past_edge_counter   = 0,
    };
    #endif

    if (0) {}

    // it's possible that a light may need 3H but not 3C,
    // so try to detect if 3C is needed
    #if NUM_CHANNEL_MODES > 1
    // 3 clicks: next channel mode
    else if (event == EV_3clicks) {
        uint8_t next = channel_mode;
        // go to next channel mode until we find one which is enabled
        // (and don't do any infinite loops if the user disabled them all)
        uint8_t count = 0;
        do {
            count ++;
            next = (next + 1) % NUM_CHANNEL_MODES;
        } while ((! channel_mode_enabled(next)) && count < NUM_CHANNEL_MODES);
        //} while ((! channel_modes_enabled[next]) && count < NUM_CHANNEL_MODES);

        // undo change if infinite loop detected (redundant?)
        //if (NUM_CHANNEL_MODES == count) next = channel_mode;

        // if mode hasn't changed, abort
        if (channel_mode == next)
            return EVENT_NOT_HANDLED;

        set_channel_mode(next);

        // remember after battery changes
        cfg.channel_mode = channel_mode;
        save_config();
        return EVENT_HANDLED;
    }
    #endif  // if NUM_CHANNEL_MODES > 1

    #ifdef USE_CUSTOM_CHANNEL_3H_MODES
    // defer to mode-specific function if defined
    else if ((event == EV_click3_hold || event == EV_click3_hold_release) &&
            channel_3H_modes[channel_mode]) {
        StatePtr tint_func = channel_3H_modes[channel_mode];
        uint8_t err = tint_func(event, arg);
        if (EVENT_HANDLED == err) return EVENT_HANDLED;
        // else let the default handler run
    }
    #endif

    #if defined(USE_CHANNEL_MODE_ARGS) && !defined(DONT_USE_DEFAULT_CHANNEL_ARG_MODE)
    // click, click, hold: change the current channel's arg (like tint)
    else if (event == EV_click3_hold) {
        if (! arg) {
            active = 1;  // first frame means this is for us
        }
        if (! active) return EVENT_NOT_HANDLED;
        return default_channel_ramp(
            arg, &ramp_state, cfg.channel_mode_args);
    }

    // click, click, hold, release: reverse direction for next ramp
    else if (event == EV_click3_hold_release) {
        active = 0;  // ignore next hold if it wasn't meant for us
        return default_channel_ramp_reverse(
            &ramp_state, cfg.channel_mode_args);
    }
    #endif  // if defined(USE_CHANNEL_MODE_ARGS) && !defined(DONT_USE_DEFAULT_CHANNEL_ARG_MODE)

    #if defined(USE_SECONDARY_CHANNEL_MODE_ARGS)
    // click, click, click, hold: change the current channel's secondary arg
    // (like saturation)
    else if (event == EV_click4_hold) {
        return default_channel_ramp(
            arg, &secondary_ramp_state,
            cfg.secondary_channel_mode_args);
    }

    // click, click, click, hold, release: reverse direction for next ramp
    else if (event == EV_click4_hold_release) {
        return default_channel_ramp_reverse(
            &secondary_ramp_state,
            cfg.secondary_channel_mode_args);
    }
    #endif  // if defined(USE_SECONDARY_CHANNEL_MODE_ARGS)

    #if defined(USE_SIMPLE_UI)
    // remaining mappings aren't "simple", so stop here
    else if (cfg.simple_ui_active) {
        return EVENT_NOT_HANDLED;
    }
    #endif

    #if NUM_CHANNEL_MODES > 1
    // channel toggle menu on ... 9H?
    else if (event == EV_click9_hold) {
        push_state(channel_mode_config_state, 0);
        return EVENT_HANDLED;
    }
    #endif

    return EVENT_NOT_HANDLED;
}

#if defined(USE_CHANNEL_MODE_ARGS) && !defined(DONT_USE_DEFAULT_CHANNEL_ARG_MODE) || defined(USE_SECONDARY_CHANNEL_MODE_ARGS)
uint8_t default_channel_ramp(uint16_t arg, RampState *state, uint8_t channel_args[]) {
    uint8_t tint = channel_args[channel_mode];

    #ifdef USE_TINT_RAMP_DIRECTION_FIX // doesn't fit on the original LT1
    ///// adjust value from 0 to 255
    // reset at beginning of movement
    if (! arg) {
        // Issue 69: if manual memory was set to an arg of 0 or 255, this could result in an incorrect
        // ramp direction with the arg resetting, so make sure tint_ramp_direction is set correctly
        if (tint == 0){
            state->tint_ramp_direction = 1;
        }
        else if (tint == 255){
            state->tint_ramp_direction = -1;
        }
    }
    #endif  // ifdef USE_TINT_RAMP_DIRECTION_FIX

    #if defined(USE_CHANNEL_MODE_ARGS) && defined(USE_STEPPED_TINT_RAMPING)
        if ((state->tint_ramp_direction > 0 && tint < 255) ||
            (state->tint_ramp_direction < 0 && tint > 0)) {
            // ramp slower in stepped mode
            if (cfg.tint_ramp_style && (arg % HOLD_TIMEOUT != 0))
                return EVENT_HANDLED;

            const uint8_t step_size = (cfg.tint_ramp_style < 2)
                                    ? 1 : 254 / (cfg.tint_ramp_style-1);
            tint = nearest_tint_value(
                      tint + ((int16_t)step_size * state->tint_ramp_direction)
                      );
        }
    #else  // smooth tint ramping only
        if ((state->tint_ramp_direction > 0) && (tint < 255)) { tint ++; }
        else
        if ((state->tint_ramp_direction < 0) && (tint >   0)) { tint --; }
    #endif  // ifdef USE_STEPPED_TINT_RAMPING

    // if tint change stalled, let user know we hit the edge
    else if (state->prev_tint == tint) {
        if (state->past_edge_counter == 0) blip();
        state->past_edge_counter = 1;
    }
    state->prev_tint = tint;
    channel_args[channel_mode] = tint;
    set_level(actual_level);
    return EVENT_HANDLED;
}

uint8_t default_channel_ramp_reverse(RampState *state, uint8_t channel_args[]) {
    uint8_t tint = channel_args[channel_mode];

    // reverse
    state->tint_ramp_direction = -state->tint_ramp_direction;
    state->past_edge_counter = 0;  // doesn't start until user hits the edge
    if (0 == tint) state->tint_ramp_direction = 1;
    else if (255 == tint) state->tint_ramp_direction = -1;
    // remember tint after battery change
    channel_args[channel_mode] = tint;
    save_config();
    // bug?: for some reason, brightness can seemingly change
    // from 1/150 to 2/150 without this next line... not sure why
    set_level(actual_level);
    return EVENT_HANDLED;
}
#endif  // if defined(USE_CHANNEL_MODE_ARGS) && !defined(DONT_USE_DEFAULT_CHANNEL_ARG_MODE) || defined(USE_SECONDARY_CHANNEL_MODE_ARGS)

#if NUM_CHANNEL_MODES > 1
void channel_mode_config_save(uint8_t step, uint8_t value) {
    // 1 menu item per channel mode, to enable or disable that mode
    step --;  // step is 1-based, channel modes are 0-based
    if (value) channel_mode_enable(step);
    else channel_mode_disable(step);
}

uint8_t channel_mode_config_state(Event event, uint16_t arg) {
    uint8_t ret;
    // make config steps match channel modes
    config_color_per_step = true;
    // 1 menu item per channel mode, to enable or disable that mode
    ret = config_state_base(
        event, arg,
        NUM_CHANNEL_MODES,
        channel_mode_config_save
        );
    // no other menu needs this
    config_color_per_step = false;
    return ret;
}
#endif


#if defined(USE_CHANNEL_MODE_ARGS) && defined(USE_STEPPED_TINT_RAMPING)
uint8_t nearest_tint_value(const int16_t target) {
    // const symbols for more readable code, will be removed by the compiler
    const uint8_t tint_min = 0;
    const uint8_t tint_max = 255;
    const uint8_t tint_range = tint_max - tint_min;

    // only equal mix of both channels
    if (1 == cfg.tint_ramp_style) return (tint_min + tint_max) >> 1;

    if (target < tint_min) return tint_min;
    if (target > tint_max) return tint_max;
    if (0 == cfg.tint_ramp_style) return target;  // smooth ramping

    const uint8_t step_size = tint_range / (cfg.tint_ramp_style-1);

    uint8_t tint_result = tint_min;
    for (uint8_t i=0; i<cfg.tint_ramp_style; i++) {
        tint_result = tint_min
                    + (i * (uint16_t)tint_range / (cfg.tint_ramp_style-1));
        int16_t diff = target - tint_result;
        if (diff <= (step_size>>1)) return tint_result;
    }
    return tint_result;
}
#endif

#ifdef USE_CIRCULAR_TINT_3H
uint8_t circular_tint_3h(Event event, uint16_t arg) {
    static int8_t tint_ramp_direction = 1;
    // bugfix: click-click-hold from off to strobes would invoke tint ramping
    // in addition to changing state...  so ignore any tint-ramp events which
    // don't look like they were meant to be here
    static uint8_t active = 0;
    uint8_t tint = cfg.channel_mode_args[channel_mode];

    // click, click, hold: change the current channel's arg (like tint)
    if (event == EV_click3_hold) {
        ///// adjust value from 0 to 255 in a circle
        // reset at beginning of movement
        if (! arg) {
            active = 1;  // first frame means this is for us
        }
        // ignore event if we weren't the ones who handled the first frame
        if (! active) return EVENT_NOT_HANDLED;

        // smooth tint ramping only
        tint += tint_ramp_direction;

        cfg.channel_mode_args[channel_mode] = tint;
        set_level(actual_level);
        return EVENT_HANDLED;
    }

    // click, click, hold, release: reverse direction for next ramp
    else if (event == EV_click3_hold_release) {
        active = 0;  // ignore next hold if it wasn't meant for us
        // reverse
        tint_ramp_direction = -tint_ramp_direction;
        // remember tint after battery change
        save_config();
        // bug?: for some reason, brightness can seemingly change
        // from 1/150 to 2/150 without this next line... not sure why
        set_level(actual_level);
        return EVENT_HANDLED;
    }

    return EVENT_NOT_HANDLED;
}
#endif
