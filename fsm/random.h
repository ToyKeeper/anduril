// fsm-random.h: Random number generator for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef USE_PSEUDO_RAND
uint8_t pseudo_rand();
// TODO: test without "volatile", in case it's not needed
volatile uint8_t pseudo_rand_seed = 0;
#endif

