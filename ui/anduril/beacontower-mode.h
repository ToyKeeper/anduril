// beacon-mode.h: Beacon mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
//               2019 loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// beacontower mode

// beacon tower flash timing (30FPM default)
uint8_t beacontower_seconds = 2;

// beacon mode
uint8_t beacontower_state(Event event, uint16_t arg);

inline void beacontower_mode_iter();
