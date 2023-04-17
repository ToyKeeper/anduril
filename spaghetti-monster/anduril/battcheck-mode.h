// battcheck-mode.h: Battery check mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

uint8_t battcheck_state(Event event, uint16_t arg);

#ifdef USE_VOLTAGE_CORRECTION
void voltage_config_save(uint8_t step, uint8_t value);
uint8_t voltage_config_state(Event event, uint16_t arg);
#endif

