// sunset-timer.h: Sunset / candle auto-shutoff functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// how many minutes to add each time the user "bumps" the timer?
#define SUNSET_TIMER_UNIT 5

#define TICKS_PER_MINUTE (TICKS_PER_SECOND*60)

// automatic shutoff timer
uint8_t sunset_timer = 0;  // minutes remaining in countdown
uint8_t sunset_timer_peak = 0;  // total minutes in countdown
uint16_t sunset_ticks = 0;  // counts from 0 to TICKS_PER_MINUTE, then repeats
uint8_t sunset_timer_state(Event event, uint16_t arg);

