// beacon-mode.c: Beacon mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "anduril/beacon-mode.h"

#if defined(USE_SMOOTH_STEPS) && (ROM_SIZE > 10000)
    #define USE_SMOOTH_BEACON
#endif

#ifdef USE_SMOOTH_BEACON
    void beacon_mode_set_level(uint8_t level, uint8_t speed) {
        if (cfg.smooth_steps_style) {
            set_level_smooth(level, speed);
            while (smooth_steps_in_progress && (! button_last_state))
                smooth_steps_iter();
        }
        else set_level(level);
    }
#else
    #define beacon_mode_set_level(level, speed)  set_level(level)
#endif

inline void beacon_mode_iter() {
    // one iteration of main loop()
    if (! button_last_state) {
        // configure behavior
        #ifdef USE_SMOOTH_BEACON
            uint16_t beacon_pulse_ms = 100;
            uint16_t beacon_anim_ms = 0;
            if (cfg.smooth_steps_style) {
                beacon_pulse_ms = 50;
                beacon_anim_ms = 450;
            }
        #else
            #define beacon_pulse_ms  100
            #define beacon_anim_ms     0
        #endif

        // turn on, and wait a short time
        beacon_mode_set_level(memorized_level, 3);
        nice_delay_ms(beacon_pulse_ms);

        // turn off, then wait for next pulse
        beacon_mode_set_level(0, 8);
        nice_delay_ms(((cfg.beacon_seconds) * 1000)
                      - beacon_pulse_ms - beacon_anim_ms);
    }
}

uint8_t beacon_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    // TODO: use sleep ticks to measure time between pulses,
    //       to save power

    // 2 clicks: next blinky mode
    else if (event == EV_2clicks) {
        #if defined(USE_SOS_MODE) && defined(USE_SOS_MODE_IN_BLINKY_GROUP)
        set_state(sos_state, 0);
        #elif defined(USE_BATTCHECK)
        set_state(battcheck_state, 0);
        #elif defined(USE_THERMAL_REGULATION)
        set_state(tempcheck_state, 0);
        #endif
        return EVENT_HANDLED;
    }
    // hold: configure beacon timing
    else if (event == EV_click1_hold) {
        if (0 == (arg % TICKS_PER_SECOND)) {
            blink_once();
        }
        return EVENT_HANDLED;
    }
    // release hold: save new timing
    else if (event == EV_click1_hold_release) {
        cfg.beacon_seconds = 1 + (arg / TICKS_PER_SECOND);
        save_config();
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}

