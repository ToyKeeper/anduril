// Regression tests for Anduril lockout momentary level selection.
// Copyright (C) 2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdint.h>
#include <stdio.h>

#include "anduril/lockout-mode-levels.h"

static int check_level(
        const char *name,
        uint8_t click_num,
        uint8_t floor_a,
        uint8_t floor_b,
        uint8_t manual_memory,
        uint8_t expected) {
    uint8_t actual = lockout_momentary_level(
            click_num,
            floor_a,
            floor_b,
            manual_memory);

    if (actual == expected) return 0;

    fprintf(stderr,
            "%s: expected %u, got %u "
            "(click=%u floor_a=%u floor_b=%u manual_memory=%u)\n",
            name,
            expected,
            actual,
            click_num,
            floor_a,
            floor_b,
            manual_memory);
    return 1;
}

int main(void) {
    int failures = 0;

    failures += check_level("1H uses stepped floor when stepped is higher",
            1, 1, 20, 0, 20);
    failures += check_level("1H uses smooth floor when smooth is higher",
            1, 25, 3, 0, 25);
    failures += check_level("2H uses highest floor without manual memory",
            2, 1, 20, 0, 20);
    failures += check_level("2H uses manual memory when configured",
            2, 1, 20, 42, 42);
    failures += check_level("1H ignores manual memory",
            1, 1, 20, 42, 20);

    return failures ? 1 : 0;
}
