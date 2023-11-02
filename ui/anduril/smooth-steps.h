// smooth-steps.h: Smooth step adjustments for Anduril.
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef USE_SMOOTH_STEPS

uint8_t smooth_steps_start;
uint8_t smooth_steps_target;
uint8_t smooth_steps_in_progress;
uint8_t smooth_steps_speed;

void smooth_steps_iter();

void set_level_smooth(uint8_t level, uint8_t speed);

#endif

