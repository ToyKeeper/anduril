// Fireflies ROT66-219 (7x7135) config options for Anduril
// Copyright (C) 2018-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as regular ROT66, but calibrated for Nichia 219B with 7x7135 chips
#include "fireflies/rot66/anduril.h"

// driver is a FET+N+1,
// where N=6 for the 219b version
// calculated to get transition points at level 65 and 115
// first 65 steps same as regular ROT66
// remaining steps from this command:
// level_calc.py ninth 3 150 7135 1 1.4 110.1 7135 14 1 1058.5 FET 1 10 4500
#undef PWM2_LEVELS
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,17,18,20,21,23,25,27,29,31,33,36,38,41,43,46,49,52,55,58,62,65,69,73,77,81,86,90,95,100,105,111,116,122,128,135,141,148,155,163,170,178,187,195,204,214,224,234,244,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#undef PWM3_LEVELS
#define PWM3_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,8,12,16,21,25,30,35,40,45,50,56,62,68,74,81,87,94,101,109,116,124,133,141,150,159,168,178,188,198,209,220,231,243,255
#undef MAX_Nx7135
#define MAX_Nx7135 115

// regulate down faster when the FET is active, slower otherwise
#undef THERM_FASTER_LEVEL
#define THERM_FASTER_LEVEL 130  // throttle back faster when high
