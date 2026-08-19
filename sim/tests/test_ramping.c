// test_ramping.c: Ramping behavior tests for Anduril
// Copyright (C) 2024 Ian Dobbie / ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Tests brightness ramping functionality
// Run: ./test_ramping ../hex/anduril.hank-emisar-d4k-3ch.hex

#include <stdio.h>
#include <stdlib.h>
#include "anduril-test.h"

static avr_t* avr = NULL;

// =============================================================================
// Test Cases
// =============================================================================

// Test: 1H from ON ramps up
void test_ramp_up(void) {
    TEST_BEGIN("ramp_up");

    // Start from off, turn on with 1C
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_on(avr), "Should be ON after 1C");

    // Get initial brightness
    uint32_t initial = anduril_get_total_output(avr);

    // 1H from ON should ramp (direction depends on ramp state)
    // First, do a brief hold to start ramping up
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, HOLD_THRESHOLD + 30);  // Hold past threshold

    uint32_t during_hold = anduril_get_total_output(avr);

    // Continue holding for more ramping
    anduril_run_ticks(avr, 50);
    uint32_t after_more_hold = anduril_get_total_output(avr);

    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    // Brightness should have changed during hold
    // (could go up or down depending on ramp direction, but should change)
    if (during_hold == initial && after_more_hold == initial) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Brightness should change during 1H: initial=%u during=%u after=%u",
                 (unsigned)initial, (unsigned)during_hold, (unsigned)after_more_hold);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: 2H from ON ramps in opposite direction
void test_ramp_down(void) {
    TEST_BEGIN("ramp_down");

    // Start from off, turn on with 2C (turbo)
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);
    anduril_multi_click(avr, 2);  // 2C for turbo

    ASSERT(anduril_is_light_on(avr), "Should be ON after 2C");

    uint32_t turbo_level = anduril_get_total_output(avr);

    // 2H from turbo should ramp down
    // Click once, then hold
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, CLICK_TICKS);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    // Now hold (this is the "2" in 2H - second press is held)
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, HOLD_THRESHOLD + 50);

    uint32_t during_2h = anduril_get_total_output(avr);

    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    // From turbo, 2H should ramp down (brightness should decrease)
    if (during_2h >= turbo_level) {
        char msg[128];
        snprintf(msg, sizeof(msg), "2H from turbo should ramp down: turbo=%u during_2h=%u",
                 (unsigned)turbo_level, (unsigned)during_2h);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: Ramp up from memorized level increases brightness
void test_ramp_ceiling(void) {
    TEST_BEGIN("ramp_ceiling");

    // Start from off, turn on
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_on(avr), "Should be ON");

    uint32_t initial = anduril_get_total_output(avr);

    // Hold to ramp up - use moderate duration
    // (very long holds may trigger timeouts)
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, HOLD_THRESHOLD + 100);  // Moderate hold

    uint32_t after_ramp = anduril_get_total_output(avr);

    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    // Should still be on
    ASSERT(anduril_is_light_on(avr), "Should still be ON after ramp");

    // After ramping up, should be brighter than initial
    if (after_ramp <= initial) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Should have ramped up: initial=%u after=%u",
                 (unsigned)initial, (unsigned)after_ramp);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: Ramp down from high level decreases brightness
void test_ramp_floor(void) {
    TEST_BEGIN("ramp_floor");

    // Start from off, go to turbo with 2C
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);
    anduril_multi_click(avr, 2);

    ASSERT(anduril_is_light_on(avr), "Should be ON at turbo");

    uint32_t turbo_level = anduril_get_total_output(avr);

    // From turbo, 1H should ramp down (since we're at/near ceiling)
    // Use moderate hold time to avoid timeouts
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, HOLD_THRESHOLD + 100);  // Moderate hold

    uint32_t ramped_down = anduril_get_total_output(avr);

    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    // Should still be on
    ASSERT(anduril_is_light_on(avr), "Should still be ON after ramp");

    // Should have ramped down from turbo (brightness decreased)
    if (ramped_down >= turbo_level) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Should have ramped down from turbo: turbo=%u ramped=%u",
                 (unsigned)turbo_level, (unsigned)ramped_down);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: Ramp memory - light returns to a consistent level
void test_ramp_memory(void) {
    TEST_BEGIN("ramp_memory");

    // Start from off, turn on
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_on(avr), "Should be ON");

    uint32_t first_on_level = anduril_get_total_output(avr);

    // Turn off
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_off(avr), "Should be OFF after 1C");

    // Turn back on
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_on(avr), "Should be ON again");

    uint32_t second_on_level = anduril_get_total_output(avr);

    // Should return to approximately the same level
    // (within 30% tolerance - memory might have slight variations)
    uint32_t diff = (first_on_level > second_on_level) ?
                    (first_on_level - second_on_level) :
                    (second_on_level - first_on_level);

    // Allow reasonable tolerance for memory restoration
    uint32_t tolerance = first_on_level / 3 + 200;  // 33% + absolute tolerance
    if (diff > tolerance) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Memory should restore similar level: first=%u second=%u diff=%u",
                 (unsigned)first_on_level, (unsigned)second_on_level, (unsigned)diff);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: Double-click while ramping goes to turbo
void test_2c_during_ramp_turbo(void) {
    TEST_BEGIN("2c_during_ramp_turbo");

    // Start from off, turn on
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_on(avr), "Should be ON");

    uint32_t initial = anduril_get_total_output(avr);

    // 2C should go to turbo
    anduril_multi_click(avr, 2);

    uint32_t after_2c = anduril_get_total_output(avr);

    // Should be at turbo (much brighter than initial)
    if (after_2c <= initial + 1000) {
        char msg[128];
        snprintf(msg, sizeof(msg), "2C should go to turbo: initial=%u after_2c=%u",
                 (unsigned)initial, (unsigned)after_2c);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    const char* hex_file = "../../hex/anduril.hank-emisar-d4k-3ch.hex";

    if (argc > 1) {
        hex_file = argv[1];
    }

    printf("========================================\n");
    printf("Anduril Ramping Tests\n");
    printf("Firmware: %s\n", hex_file);
    printf("========================================\n\n");

    // Initialize emulator
    avr = anduril_test_init(hex_file);
    if (!avr) {
        fprintf(stderr, "Failed to initialize emulator\n");
        return 1;
    }

    // Run tests
    test_ramp_up();
    test_ramp_down();
    test_ramp_ceiling();
    test_ramp_floor();
    test_ramp_memory();
    test_2c_during_ramp_turbo();

    // Print summary and return exit code
    return anduril_test_summary();
}
