// FireflyLite (Fireflies) config options for Anduril
// Copyright (C) 2018-2024 Selene ToyKeeper, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// For flashlights using the Loneoceans Lume1-6AF driver (ATTINY1616)
// - NOTE: this firmware is the no-fet variant
// - Models include: T1R, T9R, X1S, X1L, etc

// same driver hardware
#include "fireflies/lume1-6af/anduril.h"

// this is the no-fet variant
#undef RAMP_SIZE
#define RAMP_SIZE 149

// no fet-mode for turbo
#undef TURBO_TEMP_EXTRA
#define TURBO_TEMP_EXTRA 0

// reconfigure ramp tables
#undef PWM1_LEVELS
#define PWM1_LEVELS 1, 2, 7, 17, 35, 65, 111, 39, 135, 198, 206, 170, 3, 4, 14, 18, 5, 28, 34, 41, 11, 22, 26, 83, 97, 113, 131, 151, 38, 197, 82, 253, 142, 160, 179, 88, 163, 181, 200, 221, 146, 160, 176, 192, 210, 229, 249, 13, 14, 15, 17, 18, 7, 21, 5, 24, 26, 28, 30, 7, 34, 36, 14, 15, 16, 46, 49, 19, 55, 58, 62, 65, 69, 16, 28, 80, 31, 89, 94, 36, 23, 24, 25, 26, 28, 29, 30, 32, 33, 35, 36, 38, 39, 41, 43, 45, 46, 48, 50, 52, 55, 57, 59, 61, 64, 66, 69, 71, 74, 76, 79, 82, 85, 88, 91, 94, 98, 101, 104, 108, 112, 115, 119, 123, 127, 131, 135, 139, 144, 148, 153, 158, 162, 167, 172, 177, 183, 188, 194, 199, 205, 211, 217, 223, 229, 235, 242, 248, 255
#undef PWM2_LEVELS
#define PWM2_LEVELS V05, V05, V05, V05, V05, V05, V05, V25, V11, V11, V15, V25, V15, V15, V05, V05, V25, V05, V05, V05, V25, V15, V15, V05, V05, V05, V05, V05, V25, V05, V15, V05, V11, V11, V11, V25, V15, V15, V15, V15, V25, V25, V25, V25, V25, V25, V25, V05, V05, V05, V05, V05, V15, V05, V25, V05, V05, V05, V05, V25, V05, V05, V15, V15, V15, V05, V05, V15, V05, V05, V05, V05, V05, V25, V15, V05, V15, V05, V05, V15, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25, V25
#undef PWM3_LEVELS
#define PWM3_LEVELS 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
