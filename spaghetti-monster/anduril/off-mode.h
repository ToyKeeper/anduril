// off-mode.h: "Off" mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// was the light in an "on" mode within the past second or so?
uint8_t ticks_since_on = 0;

// when the light is "off" or in standby
uint8_t off_state(Event event, uint16_t arg);

