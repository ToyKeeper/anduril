// smooth-steps.c: Smooth step adjustments for Anduril.
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "smooth-steps.h"

#ifdef USE_SMOOTH_STEPS

// one iteration of main loop()
void smooth_steps_iter() {
    if (actual_level == smooth_steps_target) {
        smooth_steps_in_progress = 0;
        // restore prev_level when animation ends
        prev_level = smooth_steps_start;
        return;
    }

    if (actual_level < smooth_steps_target) {
        uint8_t diff = smooth_steps_target - actual_level;
        uint8_t this = diff / smooth_steps_speed;
        if (!this) this = 1;
        set_level(actual_level + this);
    } else if (actual_level > smooth_steps_target) {
        uint8_t diff = actual_level - smooth_steps_target;
        uint8_t this = diff / smooth_steps_speed;
        if (!this) this = 1;
        set_level(actual_level - this);
    }
    // TODO: maybe change the delay based on the speed var?
    nice_delay_ms(10);
}

void set_level_smooth(uint8_t level, uint8_t speed) {
    smooth_steps_target = level;
    // TODO: maybe speed should be a desired total time for the animation?
    smooth_steps_speed = speed;
    smooth_steps_in_progress = 1;
    // for setting prev_level after animation ends
    smooth_steps_start = actual_level;
}

#endif

