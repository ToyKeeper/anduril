// sos-mode.h: SOS mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef USE_SOS_MODE_IN_BLINKY_GROUP
// automatic SOS emergency signal
uint8_t sos_state(Event event, uint16_t arg);
#endif

