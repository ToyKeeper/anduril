// lockout-mode-fsm.h: FSM config for lockout mode in Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// autolock function requires the ability to measure time while "off"
#ifdef USE_AUTOLOCK
#define TICK_DURING_STANDBY
#endif

