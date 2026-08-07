// smooth-povd.h: Smooth post-off voltage display
// Copyright (C) 2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef USE_SMOOTH_POVD

#define smooth_povd_speed  16

uint8_t smooth_povd_state(Event event, uint16_t arg);
uint8_t calc_smooth_povd_brightness (uint8_t level);
void draw_smooth_povd (uint8_t level);

#endif

