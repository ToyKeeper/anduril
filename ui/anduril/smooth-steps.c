// smooth-steps.c: Smooth step adjustments for Anduril.
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/smooth-steps.h"

#ifdef USE_SMOOTH_STEPS

// one iteration of main loop()
void smooth_steps_iter() {
    if (actual_level == smooth_steps_target) {
        set_level(smooth_steps_target);
        smooth_steps_in_progress = 0;
        // restore prev_level when animation ends
        prev_level = smooth_steps_start;
    }
    else if (smooth_steps_target > actual_level) {
        // power-linear(ish) ascent
        // (jump by ~20% of remaining distance on each frame)
        uint8_t diff = smooth_steps_target - actual_level;
        uint8_t this = diff / smooth_steps_speed;
        if (!this) this = 1;
        set_level(actual_level + this);
        nice_delay_ms(10);
    } else {
        // ramp-linear descent
        // (jump by 1 on each frame, frame rate gives constant total time)
        uint8_t diff = smooth_steps_start - smooth_steps_target;
        uint16_t delay = 1 + (30 * smooth_steps_speed / diff);
        set_level(actual_level - 1);
        // TODO? if delay < one PWM cycle, this can look a little weird
        nice_delay_ms(delay);
    }
}

void set_level_smooth(uint8_t level, uint8_t speed) {
    smooth_steps_target = level;
    smooth_steps_speed = speed;  // higher = slower
    smooth_steps_in_progress = 1;
    // for setting prev_level after animation ends
    smooth_steps_start = actual_level;
}

#endif

