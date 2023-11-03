// sunset-timer.c: Sunset / candle auto-shutoff functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/sunset-timer.h"

uint8_t sunset_timer_state(Event event, uint16_t arg) {

    #if defined(USE_SIMPLE_UI) && !defined(USE_EXTENDED_SIMPLE_UI)
    // No timer functions in Simple UI
    if (cfg.simple_ui_active) return EVENT_NOT_HANDLED;
    #endif

    // reset on start
    if (event == EV_enter_state) {
        sunset_timer = 0;
        sunset_ticks = 0;
        return EVENT_HANDLED;
    }
    // hold: maybe "bump" the timer if it's active and almost expired
    else if (event == EV_hold) {
        // ramping up should "bump" the timer to extend the deadline a bit
        if ((sunset_timer > 0) && (sunset_timer < 4)) {
            sunset_timer = 3;  // 3 minutes
            sunset_timer_peak = 3;
            sunset_ticks = 0;  // re-start current "minute"
        }
    }
    // 5H: add 5m to timer, per second, until released
    else if (event == EV_click5_hold) {
        if (0 == (arg % TICKS_PER_SECOND)) {
            if (sunset_timer < (255 - SUNSET_TIMER_UNIT)) {
                // add a few minutes to the timer
                sunset_timer += SUNSET_TIMER_UNIT;
                sunset_timer_peak = sunset_timer;  // reset ceiling
                sunset_ticks = 0;  // reset phase
                // let the user know something happened
                blink_once();
            }
        }
        return EVENT_HANDLED;
    }
    // tick: count down until time expires
    else if (event == EV_tick) {
        // time passed
        sunset_ticks ++;
        // did we reach a minute mark?
        if (sunset_ticks >= TICKS_PER_MINUTE) {
            sunset_ticks = 0;
            if (sunset_timer > 0) {
                sunset_timer --;
            }
        }
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}

