// anduril-test.h: Test helper library for Anduril firmware testing with simavr
// Copyright (C) 2024 Ian Dobbie / ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANDURIL_TEST_H
#define ANDURIL_TEST_H

#include <stdint.h>
#include "sim_avr.h"

// =============================================================================
// Debug Verbosity
// =============================================================================

// Set to 1 to enable verbose debug output, 0 for quiet mode
// Can be overridden with -DTEST_VERBOSE=1 at compile time
#ifndef TEST_VERBOSE
#define TEST_VERBOSE 0
#endif

// =============================================================================
// Test Framework
// =============================================================================

// Test result tracking
typedef struct {
    int passed;
    int failed;
    const char* current_test;
} test_context_t;

extern test_context_t test_ctx;

// Test macros
#define TEST_BEGIN(name) do { \
    test_ctx.current_test = name; \
    printf("  TEST: %s... ", name); \
    fflush(stdout); \
} while(0)

#define TEST_PASS() do { \
    printf("PASS\n"); \
    test_ctx.passed++; \
} while(0)

#define TEST_FAIL(msg) do { \
    printf("FAIL: %s\n", msg); \
    test_ctx.failed++; \
} while(0)

#define ASSERT(cond, msg) do { \
    if (!(cond)) { TEST_FAIL(msg); return; } \
} while(0)

#define ASSERT_EQ(a, b, msg) ASSERT((a) == (b), msg)
#define ASSERT_NE(a, b, msg) ASSERT((a) != (b), msg)
#define ASSERT_GT(a, b, msg) ASSERT((a) > (b), msg)
#define ASSERT_LT(a, b, msg) ASSERT((a) < (b), msg)

// =============================================================================
// PWM State
// =============================================================================

typedef struct {
    uint16_t main2;   // Main LEDs (OCR0A, 8-bit scaled to ~15-bit)
    uint16_t led3;    // LED3 (OCR1A, 16-bit)
    uint16_t led4;    // LED4 (OCR1B, 16-bit)
    uint8_t  aux_r;   // Aux red LED: 0=off, 1=low(pullup), 2=high
    uint8_t  aux_g;   // Aux green LED: 0=off, 1=low(pullup), 2=high
    uint8_t  aux_b;   // Aux blue LED: 0=off, 1=low(pullup), 2=high
} pwm_state_t;

// =============================================================================
// Timing Constants
// =============================================================================

// WDT tick period in cycles (16ms at 8MHz = 128000 cycles)
#define WDT_CYCLES      128000

// Button timing (in WDT ticks, ~16ms each)
// Anduril uses ~16ms WDT ticks, click timeout is typically 64 ticks (~1s)
#define CLICK_TICKS     5       // ~80ms click duration
#define CLICK_GAP_TICKS 8       // ~128ms gap between clicks
#define HOLD_THRESHOLD  25      // ~400ms to register as hold
#define LONG_TIMEOUT    150     // ~2.4s for timeout events (click processing)

// =============================================================================
// Initialization
// =============================================================================

// Initialize AVR emulator with firmware hex file
// Returns NULL on failure
avr_t* anduril_test_init(const char* hex_path);

// Clean up emulator resources
void anduril_test_cleanup(avr_t* avr);

// Reset emulator to initial state (for running multiple tests)
void anduril_test_reset(avr_t* avr);

// =============================================================================
// Time Advancement
// =============================================================================

// Run emulator for specified number of cycles
void anduril_run_cycles(avr_t* avr, uint64_t cycles);

// Run emulator for specified number of WDT ticks (~16ms each)
void anduril_run_ticks(avr_t* avr, int ticks);

// Run emulator for specified milliseconds
void anduril_run_ms(avr_t* avr, int ms);

// =============================================================================
// Button Control
// =============================================================================

// Set button state (pressed = 1, released = 0)
void anduril_button_set(avr_t* avr, int pressed);

// Perform a click (press + release)
void anduril_click(avr_t* avr);

// Perform multiple clicks (double-click, triple-click, etc.)
void anduril_multi_click(avr_t* avr, int count);

// Perform a hold (press, wait ticks, release)
void anduril_hold(avr_t* avr, int ticks);

// Perform click then hold (1H, 2H, 3H patterns)
void anduril_click_hold(avr_t* avr, int clicks, int hold_ticks);

// =============================================================================
// PWM Reading
// =============================================================================

// Read current PWM values from emulator registers
pwm_state_t anduril_get_pwm(avr_t* avr);

// Check if any LED is on
int anduril_is_light_on(avr_t* avr);

// Check if all LEDs are off
int anduril_is_light_off(avr_t* avr);

// Get total light output (sum of all channels)
uint32_t anduril_get_total_output(avr_t* avr);

// =============================================================================
// Test Assertions
// =============================================================================

// Assert light is on (any PWM > 0)
void assert_light_on(avr_t* avr);

// Assert light is off (all PWM = 0)
void assert_light_off(avr_t* avr);

// Assert PWM is in specified range
void assert_pwm_range(avr_t* avr, uint16_t min_total, uint16_t max_total);

// Assert specific channel values
void assert_main2_range(avr_t* avr, uint16_t min, uint16_t max);
void assert_led3_range(avr_t* avr, uint16_t min, uint16_t max);
void assert_led4_range(avr_t* avr, uint16_t min, uint16_t max);

// =============================================================================
// ADC Control
// =============================================================================

// Set battery voltage (in millivolts)
void anduril_set_voltage(avr_t* avr, uint16_t millivolts);

// Get current battery voltage reading (in millivolts)
uint16_t anduril_get_voltage(avr_t* avr);

// Set temperature (in Celsius)
void anduril_set_temperature(avr_t* avr, int8_t celsius);

// =============================================================================
// EEPROM Access
// =============================================================================

// Read single byte from EEPROM
uint8_t anduril_eeprom_read(avr_t* avr, uint16_t offset);

// Write single byte to EEPROM
void anduril_eeprom_write(avr_t* avr, uint16_t offset, uint8_t value);

// Dump EEPROM region to buffer
void anduril_eeprom_dump(avr_t* avr, uint8_t* buffer, uint16_t offset, uint16_t length);

// =============================================================================
// Test Suite Helpers
// =============================================================================

// Print test suite summary and return exit code
int anduril_test_summary(void);

// Print current PWM state (for debugging)
void anduril_print_pwm(avr_t* avr);

#endif // ANDURIL_TEST_H
