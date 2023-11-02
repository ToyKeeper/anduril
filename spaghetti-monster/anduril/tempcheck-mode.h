// tempcheck-mode.h: Temperature check mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define USE_BLINK_NUM  // FIXME: this only matters in an earlier header

uint8_t tempcheck_state(Event event, uint16_t arg);
uint8_t thermal_config_state(Event event, uint16_t arg);
void thermal_config_save(uint8_t step, uint8_t value);

