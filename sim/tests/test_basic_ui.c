// test_basic_ui.c: Core UI behavior tests for Anduril
// Copyright (C) 2024 Ian Dobbie / ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Tests basic on/off, click, and hold functionality
// Run: ./test_basic_ui ../hex/anduril.hank-emisar-d4k-3ch.hex

#include <stdio.h>
#include <stdlib.h>
#include "anduril-test.h"

static avr_t* avr = NULL;

// =============================================================================
// Test Cases
// =============================================================================

// Test: Light starts in OFF state after power-on
void test_starts_off(void) {
    TEST_BEGIN("starts_off");

    // Reset and let it initialize
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);  // Let startup complete (needs more time for init)

    // Should be off
    if (!anduril_is_light_off(avr)) {
        pwm_state_t pwm = anduril_get_pwm(avr);
        char msg[128];
        snprintf(msg, sizeof(msg), "Expected OFF after startup, got main2=%d led3=%d led4=%d",
                 pwm.main2, pwm.led3, pwm.led4);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: Single click turns light ON
void test_click_on(void) {
    TEST_BEGIN("click_on");

    // Start from off state
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);  // Let startup complete

    // Verify we're off first
    ASSERT(anduril_is_light_off(avr), "Should start OFF");

    // Single click
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);  // Wait for state machine to process

    // Should be on now
    if (!anduril_is_light_on(avr)) {
        anduril_print_pwm(avr);
        TEST_FAIL("Light should be ON after click");
        return;
    }

    TEST_PASS();
}

// Test: Click while ON turns light OFF
void test_click_off(void) {
    TEST_BEGIN("click_off");

    // Start from off, click to turn on
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 10);
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    // Verify we're on
    ASSERT(anduril_is_light_on(avr), "Should be ON before second click");

    // Second click to turn off
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    // Should be off now
    if (!anduril_is_light_off(avr)) {
        anduril_print_pwm(avr);
        TEST_FAIL("Light should be OFF after second click");
        return;
    }

    TEST_PASS();
}

// Test: Hold from OFF starts moon mode and ramps
void test_hold_from_off(void) {
    TEST_BEGIN("hold_from_off");

    // Start from off - need enough time for firmware to fully initialize
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);  // Give firmware time to initialize
    ASSERT(anduril_is_light_off(avr), "Should start OFF");

    // Hold button - should start ramping
    // Need to hold longer than just threshold to see moon/ramp start
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, HOLD_THRESHOLD + 50);  // Hold ~1.2s total

    // Should be on now (at moon or ramping)
    if (!anduril_is_light_on(avr)) {
        anduril_print_pwm(avr);
        TEST_FAIL("Light should be ON during hold");
        return;
    }

    // Get initial brightness
    uint32_t initial = anduril_get_total_output(avr);

    // Continue holding - brightness should increase
    anduril_run_ticks(avr, 30);
    uint32_t final = anduril_get_total_output(avr);

    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    // Check that brightness increased
    if (final <= initial) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Brightness should increase during ramp: %u -> %u",
                 (unsigned)initial, (unsigned)final);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: Double-click for turbo
void test_double_click_turbo(void) {
    TEST_BEGIN("double_click_turbo");

    // Start from off
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 10);

    // Double-click
    anduril_multi_click(avr, 2);

    // Should be at high brightness (turbo)
    if (!anduril_is_light_on(avr)) {
        TEST_FAIL("Light should be ON after 2C");
        return;
    }

    uint32_t output = anduril_get_total_output(avr);
    // Turbo should be fairly bright - at least 5000 in total
    // (D4K-3ch turbo is around 7000-8000 with default settings)
    if (output < 5000) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Expected high output for turbo, got %u",
                 (unsigned)output);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: 4-click enters lockout mode
void test_4click_lockout(void) {
    TEST_BEGIN("4click_lockout");

    // Start from off - need enough startup time
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);  // Give firmware time to initialize

    // Verify we're off
    ASSERT(anduril_is_light_off(avr), "Should start OFF");

    // 4 clicks to enter lockout
    anduril_multi_click(avr, 4);

    // Wait for lockout mode to settle (blink_once + state transition)
    anduril_run_ticks(avr, 50);

    // Light should still be off (lockout mode)
    // Note: Aux LEDs might be on, but main LEDs should be off
    pwm_state_t pwm = anduril_get_pwm(avr);

    // In lockout, main PWM channels should be off
    if (pwm.main2 > 0 || pwm.led3 > 0 || pwm.led4 > 0) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Main LEDs should be off in lockout: main2=%d led3=%d led4=%d",
                 pwm.main2, pwm.led3, pwm.led4);
        TEST_FAIL(msg);
        return;
    }

    // Note: With USE_MOON_DURING_LOCKOUT_MODE, a 1C in lockout momentarily shows moon.
    // Testing that behavior requires more complex timing verification.
    // This test just verifies that 4C enters lockout (light stays off).

    TEST_PASS();
}

// Test: 4-click from lockout exits lockout
void test_4click_unlock(void) {
    TEST_BEGIN("4click_unlock");

    // Start from off - need enough startup time
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);  // Give firmware time to initialize

    // Enter lockout
    anduril_multi_click(avr, 4);
    anduril_run_ticks(avr, 20);  // Let lockout settle

    // Exit lockout with 4 clicks
    anduril_multi_click(avr, 4);
    anduril_run_ticks(avr, 20);  // Let state settle

    // Now click should turn on
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    if (!anduril_is_light_on(avr)) {
        TEST_FAIL("Light should turn ON after exiting lockout");
        return;
    }

    TEST_PASS();
}

// Test: Hold while ON ramps down (2H)
void test_hold_while_on_ramps(void) {
    TEST_BEGIN("hold_while_on_ramps");

    // Start from off, turn on
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 10);
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_on(avr), "Should be ON");

    // Get current brightness
    uint32_t initial = anduril_get_total_output(avr);

    // 1H from ON should ramp up (or down, depending on direction)
    anduril_hold(avr, HOLD_THRESHOLD + 20);

    uint32_t after_1h = anduril_get_total_output(avr);

    // Brightness should have changed
    if (after_1h == initial) {
        TEST_FAIL("Brightness should change during hold");
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
    printf("Anduril Basic UI Tests\n");
    printf("Firmware: %s\n", hex_file);
    printf("========================================\n\n");

    // Initialize emulator
    avr = anduril_test_init(hex_file);
    if (!avr) {
        fprintf(stderr, "Failed to initialize emulator\n");
        return 1;
    }

    // Run tests
    test_starts_off();
    test_click_on();
    test_click_off();
    test_hold_from_off();
    test_double_click_turbo();
    test_4click_lockout();
    test_4click_unlock();
    test_hold_while_on_ramps();

    // Print summary and return exit code
    return anduril_test_summary();
}
