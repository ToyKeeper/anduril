// Noctigon K9.3 (reduced FET) config options for Anduril
// Copyright (C) 2020-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "cfg-noctigon-k9.3.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0263"
// ATTINY: 1634

// main LEDs
#undef PWM2_LEVELS
// 65% FET power
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,5,7,10,13,16,18,22,24,27,31,34,37,41,44,48,52,55,59,63,68,72,76,80,85,89,94,99,104,109,114,119,124,130,135,141,147,153,159,165

