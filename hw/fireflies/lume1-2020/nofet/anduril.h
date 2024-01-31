// FireflyLite (Fireflies) config options for Anduril
// Copyright (C) 2018-2023 Selene ToyKeeper, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// for flashlights using the Loneoceans Lume1 driver (ATTINY1634)
// this firmware variant is for no-fet! 
// this driver has been superseded by the Lume1-6AF (ATTINY1616)

// same driver hardware
#include "fireflies/lume1-2020/anduril.h"

// this is the no-fet variant
#undef RAMP_SIZE
#define RAMP_SIZE 149

// reconfigure ramp tables
#undef PWM1_LEVELS
#define PWM1_LEVELS 16,17,17,18,18,19,20,21,22,23,24,25,26,28,30,31,33,35,38,40,43,46,49,52,56,60,64,68,73,79,84,90,97,104,112,120,128,137,147,158,169,181,194,207,222,237,253,270,289,308,329,350,374,398,424,451,480,510,543,576,612,650,690,731,775,822,870,921,975,1032,1091,1153,1218,1287,1358,1434,1512,1595,1681,1771,1866,1965,2068,2176,2289,2406,2529,2657,2791,2931,3077,3228,3387,3551,3723,3902,4088,4281,4483,4693,4911,5137,5373,5617,5872,6136,6410,6695,6990,7297,7615,7945,8287,8642,9009,9390,9785,10194,10618,11056,11510,11980,12466,12969,13490,14028,14584,15160,15754,16369,17004,17660,18338,19038,19760,20507,21277,22071,22892,23738,24610,25511,26439,27396,28383,29399,30448,31527,32640
// DD FET: 8-bit PWM (hardware can handle PWM but preference is on or off)
#undef PWM2_LEVELS
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

// no fet-mode for turbo
#undef TURBO_TEMP_EXTRA