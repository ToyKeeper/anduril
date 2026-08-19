// test_channel_modes.c: Channel mode tests for Anduril
// Copyright (C) 2024 Ian Dobbie / ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Tests channel mode switching and behavior
// Run: ./test_channel_modes ../hex/anduril.hank-emisar-d4k-3ch.hex

#include <stdio.h>
#include <stdlib.h>
#include "anduril-test.h"

static avr_t* avr = NULL;

// 3C = next channel mode in Anduril's UI (3H adjusts the mode's tint/arg
// instead, and in argless modes falls through to momentary turbo)
static void next_channel_mode(void) {
    anduril_multi_click(avr, 3);
    anduril_run_ticks(avr, LONG_TIMEOUT);
}

// The D4K-3ch drives its LEDs by delta-sigma modulation, so raw PWM
// samples jitter by one high-byte step (~128 on main2, ~1 on led3/led4)
// even in a steady state.  Compare patterns by weighted distance with a
// threshold safely above that dither instead of by exact equality.
static int pwm_distance(pwm_state_t a, pwm_state_t b) {
    int d = abs((int)a.main2 - (int)b.main2);
    d += 16 * abs((int)a.led3 - (int)b.led3);
    d += 16 * abs((int)a.led4 - (int)b.led4);
    return d;
}
#define PWM_CHANGE_THRESHOLD 300

// =============================================================================
// Test Cases
// =============================================================================

// Test: 3C from ON changes channel mode (PWM pattern changes)
void test_channel_switch(void) {
    TEST_BEGIN("channel_switch");

    // Start from off, turn on with 1C
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_on(avr), "Should be ON after 1C");

    // Get initial PWM state (default channel mode)
    pwm_state_t initial = anduril_get_pwm(avr);

    // 3C switches to the next channel mode
    next_channel_mode();

    // Should still be on
    ASSERT(anduril_is_light_on(avr), "Should be ON after 3C");

    // Get new PWM state
    pwm_state_t after_3c = anduril_get_pwm(avr);

    // Channel mode should have changed (PWM pattern different)
    if (pwm_distance(initial, after_3c) < PWM_CHANGE_THRESHOLD) {
        char msg[200];
        snprintf(msg, sizeof(msg),
                 "3C should change channel mode: before(m2=%u l3=%u l4=%u) after(m2=%u l3=%u l4=%u)",
                 initial.main2, initial.led3, initial.led4,
                 after_3c.main2, after_3c.led3, after_3c.led4);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: Channel mode persists across on/off cycle
void test_channel_mode_persists(void) {
    TEST_BEGIN("channel_mode_persists");

    // Start from off, turn on
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_on(avr), "Should be ON");

    // Switch channel mode with 3C
    next_channel_mode();

    // Record PWM state after channel switch
    pwm_state_t after_switch = anduril_get_pwm(avr);

    // Turn off
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);
    ASSERT(anduril_is_light_off(avr), "Should be OFF");

    // Turn back on
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);
    ASSERT(anduril_is_light_on(avr), "Should be ON again");

    // Get PWM state
    pwm_state_t after_restore = anduril_get_pwm(avr);

    // Channel mode should persist (same general pattern)
    // Allow some tolerance since brightness might differ slightly
    // Check that the same channels are active/inactive

    int switch_has_main2 = (after_switch.main2 > 100);
    int switch_has_led3 = (after_switch.led3 > 100);
    int switch_has_led4 = (after_switch.led4 > 100);

    int restore_has_main2 = (after_restore.main2 > 100);
    int restore_has_led3 = (after_restore.led3 > 100);
    int restore_has_led4 = (after_restore.led4 > 100);

    if (switch_has_main2 != restore_has_main2 ||
        switch_has_led3 != restore_has_led3 ||
        switch_has_led4 != restore_has_led4) {
        char msg[200];
        snprintf(msg, sizeof(msg),
                 "Channel mode should persist: switch(m2=%d l3=%d l4=%d) restore(m2=%d l3=%d l4=%d)",
                 switch_has_main2, switch_has_led3, switch_has_led4,
                 restore_has_main2, restore_has_led3, restore_has_led4);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: Multiple 3C presses cycle through different channel modes
void test_channel_cycles(void) {
    TEST_BEGIN("channel_cycles");

    // Start from off, turn on
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_on(avr), "Should be ON");

    pwm_state_t modes[4];
    modes[0] = anduril_get_pwm(avr);

    // Do 3C three more times to collect different mode patterns
    for (int i = 1; i < 4; i++) {
        next_channel_mode();

        ASSERT(anduril_is_light_on(avr), "Should still be ON after 3C");

        modes[i] = anduril_get_pwm(avr);
    }

    // Count how many clearly-distinct patterns we got
    int unique_patterns = 1;
    for (int i = 1; i < 4; i++) {
        int is_unique = 1;
        for (int j = 0; j < i; j++) {
            if (pwm_distance(modes[i], modes[j]) < PWM_CHANGE_THRESHOLD) {
                is_unique = 0;
                break;
            }
        }
        if (is_unique) unique_patterns++;
    }

    // Should have at least 2 different channel mode patterns
    // (Some modes might look similar due to brightness matching)
    if (unique_patterns < 2) {
        char msg[256];
        snprintf(msg, sizeof(msg),
                 "Expected multiple channel modes, got %d unique patterns: "
                 "[0](m2=%u l3=%u l4=%u) [1](m2=%u l3=%u l4=%u) "
                 "[2](m2=%u l3=%u l4=%u) [3](m2=%u l3=%u l4=%u)",
                 unique_patterns,
                 modes[0].main2, modes[0].led3, modes[0].led4,
                 modes[1].main2, modes[1].led3, modes[1].led4,
                 modes[2].main2, modes[2].led3, modes[2].led4,
                 modes[3].main2, modes[3].led3, modes[3].led4);
        TEST_FAIL(msg);
        return;
    }

    TEST_PASS();
}

// Test: Different channel modes have different LED combinations
void test_channel_mode_leds(void) {
    TEST_BEGIN("channel_mode_leds");

    // Start from off, turn on
    anduril_test_reset(avr);
    anduril_run_ticks(avr, 50);
    anduril_click(avr);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    ASSERT(anduril_is_light_on(avr), "Should be ON");

    // Default mode (CM_ALL on the D4K-3ch) drives main2
    pwm_state_t mode0 = anduril_get_pwm(avr);

    // Expect main2 to be dominant in default mode
    // Note: This is D4K-3ch specific
    if (mode0.main2 == 0) {
        char msg[128];
        snprintf(msg, sizeof(msg),
                 "Default mode should have main2 active: m2=%u l3=%u l4=%u",
                 mode0.main2, mode0.led3, mode0.led4);
        TEST_FAIL(msg);
        return;
    }

    // 3C switches to the next mode
    next_channel_mode();

    // After 3C, should be in a different mode with a different LED mix
    pwm_state_t mode1 = anduril_get_pwm(avr);

    if (pwm_distance(mode0, mode1) < PWM_CHANGE_THRESHOLD) {
        char msg[200];
        snprintf(msg, sizeof(msg),
                 "LED pattern should change after 3C: before(m2=%u l3=%u l4=%u) after(m2=%u l3=%u l4=%u)",
                 mode0.main2, mode0.led3, mode0.led4,
                 mode1.main2, mode1.led3, mode1.led4);
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
    printf("Anduril Channel Mode Tests\n");
    printf("Firmware: %s\n", hex_file);
    printf("========================================\n\n");

    // Initialize emulator
    avr = anduril_test_init(hex_file);
    if (!avr) {
        fprintf(stderr, "Failed to initialize emulator\n");
        return 1;
    }

    // Run tests
    test_channel_switch();
    test_channel_mode_persists();
    test_channel_cycles();
    test_channel_mode_leds();

    // Print summary and return exit code
    return anduril_test_summary();
}
