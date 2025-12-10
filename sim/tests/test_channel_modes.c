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

// =============================================================================
// Test Cases
// =============================================================================

// Test: 3H from ON changes channel mode (PWM pattern changes)
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

    // 3H should switch channel modes
    // First do 2 clicks, then hold
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, CLICK_TICKS);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, CLICK_TICKS);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    // Third press - hold
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, HOLD_THRESHOLD + 30);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    // Should still be on
    ASSERT(anduril_is_light_on(avr), "Should be ON after 3H");

    // Get new PWM state
    pwm_state_t after_3h = anduril_get_pwm(avr);

    // Channel mode should have changed (PWM pattern different)
    // At minimum, the ratio between channels should change
    int same_pattern = (initial.main2 == after_3h.main2 &&
                        initial.led3 == after_3h.led3 &&
                        initial.led4 == after_3h.led4);

    if (same_pattern) {
        char msg[200];
        snprintf(msg, sizeof(msg),
                 "3H should change channel mode: before(m2=%u l3=%u l4=%u) after(m2=%u l3=%u l4=%u)",
                 initial.main2, initial.led3, initial.led4,
                 after_3h.main2, after_3h.led3, after_3h.led4);
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

    // Switch channel mode with 3H (2C then hold)
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, CLICK_TICKS);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, CLICK_TICKS);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, HOLD_THRESHOLD + 30);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, LONG_TIMEOUT);

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

// Test: Multiple 3H cycles through different channel modes
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

    // Do 3H three more times to collect different mode patterns
    for (int i = 1; i < 4; i++) {
        // 3H: 2C then hold
        anduril_button_set(avr, 1);
        anduril_run_ticks(avr, CLICK_TICKS);
        anduril_button_set(avr, 0);
        anduril_run_ticks(avr, CLICK_GAP_TICKS);

        anduril_button_set(avr, 1);
        anduril_run_ticks(avr, CLICK_TICKS);
        anduril_button_set(avr, 0);
        anduril_run_ticks(avr, CLICK_GAP_TICKS);

        anduril_button_set(avr, 1);
        anduril_run_ticks(avr, HOLD_THRESHOLD + 30);
        anduril_button_set(avr, 0);
        anduril_run_ticks(avr, LONG_TIMEOUT);

        ASSERT(anduril_is_light_on(avr), "Should still be ON after 3H");

        modes[i] = anduril_get_pwm(avr);
    }

    // Count how many unique patterns we got
    int unique_patterns = 1;
    for (int i = 1; i < 4; i++) {
        int is_unique = 1;
        for (int j = 0; j < i; j++) {
            if (modes[i].main2 == modes[j].main2 &&
                modes[i].led3 == modes[j].led3 &&
                modes[i].led4 == modes[j].led4) {
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

    // Default mode (CM_MAIN2) should primarily use main2
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

    // Do 3H to switch to next mode (CM_LED3)
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, CLICK_TICKS);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, CLICK_TICKS);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);

    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, HOLD_THRESHOLD + 30);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, LONG_TIMEOUT);

    // After 3H, should be in a different mode with different LED active
    pwm_state_t mode1 = anduril_get_pwm(avr);

    // At least one channel should have changed
    int channel_changed = (mode0.main2 != mode1.main2 ||
                           mode0.led3 != mode1.led3 ||
                           mode0.led4 != mode1.led4);

    if (!channel_changed) {
        char msg[200];
        snprintf(msg, sizeof(msg),
                 "LED pattern should change after 3H: before(m2=%u l3=%u l4=%u) after(m2=%u l3=%u l4=%u)",
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
