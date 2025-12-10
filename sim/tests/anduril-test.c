// anduril-test.c: Test helper library implementation
// Copyright (C) 2024 Ian Dobbie / ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "anduril-test.h"
#include "sim_avr.h"
#include "sim_elf.h"
#include "sim_hex.h"
#include "sim_irq.h"
#include "avr_ioport.h"

// =============================================================================
// ATtiny1634 Register Addresses
// =============================================================================

// Timer/Counter registers for PWM (ATtiny1634)
// OCR0A is _SFR_IO8(0x18) = I/O 0x18 + 0x20 = 0x38
// OCR1x are _SFR_MEM8 (already memory addresses)
#define OCR0A_ADDR  0x38  // Timer0 Compare A (8-bit, main2)
#define OCR1AL_ADDR 0x6C  // Timer1 Compare A Low (16-bit, led3)
#define OCR1AH_ADDR 0x6D  // Timer1 Compare A High
#define OCR1BL_ADDR 0x6A  // Timer1 Compare B Low (16-bit, led4)
#define OCR1BH_ADDR 0x6B  // Timer1 Compare B High

// I/O Port registers (I/O address + 0x20 for data memory access)
// From iotn1634.h: PINA=0x0F, DDRA=0x10, PORTA=0x11
#define PINA_ADDR   0x2F  // Port A Input Pins (0x0F + 0x20)
#define DDRA_ADDR   0x30  // Port A Data Direction (0x10 + 0x20)
#define PORTA_ADDR  0x31  // Port A Data Register (0x11 + 0x20)

// PCINT registers (I/O address + 0x20 for data memory access)
// From iotn1634.h: GIMSK=0x3C, GIFR=0x3B, PCMSK0=0x27
#define GIMSK_ADDR  0x5C  // General Interrupt Mask (0x3C + 0x20)
#define GIFR_ADDR   0x5B  // General Interrupt Flag (0x3B + 0x20)
#define PCMSK0_ADDR 0x47  // Pin Change Mask 0 (0x27 + 0x20)

// Button pin
#define SWITCH_PIN  7     // PA7

// =============================================================================
// Global Test Context
// =============================================================================

test_context_t test_ctx = {0, 0, NULL};

// =============================================================================
// Initialization
// =============================================================================

avr_t* anduril_test_init(const char* hex_path) {
    // Create AVR instance for ATtiny1634
    avr_t* avr = avr_make_mcu_by_name("attiny1634");
    if (!avr) {
        fprintf(stderr, "Error: Failed to create AVR instance for attiny1634\n");
        return NULL;
    }

    avr_init(avr);
    avr->frequency = 8000000;  // 8 MHz

    // Load the hex file
    uint32_t loadBase = AVR_SEGMENT_OFFSET_FLASH;
    uint32_t loadSize;
    uint8_t* firmware = read_ihex_file(hex_path, &loadSize, &loadBase);
    if (!firmware) {
        fprintf(stderr, "Error: Failed to load hex file: %s\n", hex_path);
        return NULL;
    }

    // Copy firmware to AVR flash
    memcpy(avr->flash + loadBase, firmware, loadSize);
    free(firmware);

    // Reset the AVR
    avr_reset(avr);

    // Initialize button as released (PA7 high, active low)
    anduril_button_set(avr, 0);

    return avr;
}

void anduril_test_cleanup(avr_t* avr) {
    if (avr) {
        // Note: simavr doesn't have a proper cleanup function
        // The memory will be freed when process exits
    }
}

void anduril_test_reset(avr_t* avr) {
    if (avr) {
        avr_reset(avr);
        anduril_button_set(avr, 0);
    }
}

// =============================================================================
// Time Advancement
// =============================================================================

void anduril_run_cycles(avr_t* avr, uint64_t cycles) {
    uint64_t target = avr->cycle + cycles;
    static int pcint_count = 0;
    static int wdt_count = 0;

    while (avr->cycle < target) {
        int state = avr_run(avr);
        if (state == cpu_Done || state == cpu_Crashed) {
            fprintf(stderr, "Warning: AVR stopped during run (state=%d, pc=0x%04x)\n",
                    state, avr->pc);
            break;
        }

        // Trace key interrupt vectors (limited output, verbose mode only)
        #if TEST_VERBOSE
        if (avr->pc == 0x0008 && pcint_count < 5) {  // PCINT0
            printf("  ** PCINT fired! PINA=0x%02x cycle=%llu\n",
                   avr->data[PINA_ADDR], (unsigned long long)avr->cycle);
            pcint_count++;
        }
        if (avr->pc == 0x0014 && wdt_count < 5) {  // WDT
            printf("  ** WDT fired! cycle=%llu\n", (unsigned long long)avr->cycle);
            wdt_count++;
        }
        #else
        (void)pcint_count;  // Suppress unused variable warning
        (void)wdt_count;
        #endif
    }
}

void anduril_run_ticks(avr_t* avr, int ticks) {
    anduril_run_cycles(avr, (uint64_t)ticks * WDT_CYCLES);
}

void anduril_run_ms(avr_t* avr, int ms) {
    // 8 MHz = 8000 cycles per ms
    anduril_run_cycles(avr, (uint64_t)ms * 8000);
}

// =============================================================================
// Button Control
// =============================================================================

void anduril_button_set(avr_t* avr, int pressed) {
    // PA7 is the switch pin, active low
    // Simplified approach: directly manipulate PIN register and trigger PCINT

    // Get current PINA value
    uint8_t old_pina = avr->data[PINA_ADDR];
    uint8_t new_pina;

    if (pressed) {
        new_pina = old_pina & ~(1 << SWITCH_PIN);  // Clear bit 7 (LOW = pressed)
    } else {
        new_pina = old_pina | (1 << SWITCH_PIN);   // Set bit 7 (HIGH = released)
    }

    // Check if state actually changed
    int state_changed = ((old_pina ^ new_pina) & (1 << SWITCH_PIN)) != 0;

    // Write new value directly to PIN register
    avr->data[PINA_ADDR] = new_pina;

    // Also set external pull value (backup mechanism)
    avr_ioport_external_t ext = {
        .name = 'A',
        .mask = (1 << SWITCH_PIN),
        .value = pressed ? 0 : (1 << SWITCH_PIN)
    };
    avr_ioctl(avr, AVR_IOCTL_IOPORT_SET_EXTERNAL('A'), &ext);

    // Trigger PCINT if state changed and interrupts are configured
    if (state_changed) {
        uint8_t gimsk = avr->data[GIMSK_ADDR];
        uint8_t pcmsk0 = avr->data[PCMSK0_ADDR];

        // Check if PCINT is enabled for this pin
        // ATtiny1634: GIMSK bit 3 = PCIE0 (Pin Change Interrupt Enable 0)
        // PCMSK0 bit 7 = PCINT7 (PA7)
        if ((gimsk & (1 << 3)) && (pcmsk0 & (1 << SWITCH_PIN))) {
            // Set PCINT flag (ATtiny1634: GIFR bit 3 = PCIF0)
            avr->data[GIFR_ADDR] |= (1 << 3);

            // Also raise the IRQ to trigger simavr's interrupt mechanism
            avr_irq_t* irq = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('A'), SWITCH_PIN);
            if (irq) {
                avr_raise_irq(irq, pressed ? 0 : 1);
            }
        }
    }

    // Debug output (verbose mode only)
    #if TEST_VERBOSE
    static int count = 0;
    if (count++ < 10) {
        uint8_t ddra = avr->data[DDRA_ADDR];
        uint8_t gimsk = avr->data[GIMSK_ADDR];
        uint8_t pcmsk0 = avr->data[PCMSK0_ADDR];
        printf("  button(%d): PINA 0x%02x->0x%02x DDRA=0x%02x GIMSK=0x%02x PCMSK0=0x%02x changed=%d cycle=%llu\n",
               pressed, old_pina, new_pina, ddra, gimsk, pcmsk0, state_changed, (unsigned long long)avr->cycle);
    }
    #endif
}

void anduril_click(avr_t* avr) {
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, CLICK_TICKS);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);
}

void anduril_multi_click(avr_t* avr, int count) {
    for (int i = 0; i < count; i++) {
        anduril_button_set(avr, 1);
        anduril_run_ticks(avr, CLICK_TICKS);
        anduril_button_set(avr, 0);
        if (i < count - 1) {
            anduril_run_ticks(avr, CLICK_GAP_TICKS);
        }
    }
    // Wait for click sequence to be processed
    anduril_run_ticks(avr, LONG_TIMEOUT);
}

void anduril_hold(avr_t* avr, int ticks) {
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, ticks);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);
}

void anduril_click_hold(avr_t* avr, int clicks, int hold_ticks) {
    // Do the clicks first
    for (int i = 0; i < clicks; i++) {
        anduril_button_set(avr, 1);
        anduril_run_ticks(avr, CLICK_TICKS);
        anduril_button_set(avr, 0);
        anduril_run_ticks(avr, CLICK_GAP_TICKS);
    }
    // Then hold
    anduril_button_set(avr, 1);
    anduril_run_ticks(avr, hold_ticks);
    anduril_button_set(avr, 0);
    anduril_run_ticks(avr, CLICK_GAP_TICKS);
}

// =============================================================================
// PWM Reading
// =============================================================================

pwm_state_t anduril_get_pwm(avr_t* avr) {
    pwm_state_t pwm;

    // Read 8-bit OCR0A (main2), scale to ~15-bit
    pwm.main2 = avr->data[OCR0A_ADDR] << 7;

    // Read 16-bit OCR1A (led3)
    pwm.led3 = avr->data[OCR1AL_ADDR] | (avr->data[OCR1AH_ADDR] << 8);

    // Read 16-bit OCR1B (led4)
    pwm.led4 = avr->data[OCR1BL_ADDR] | (avr->data[OCR1BH_ADDR] << 8);

    return pwm;
}

int anduril_is_light_on(avr_t* avr) {
    pwm_state_t pwm = anduril_get_pwm(avr);
    return (pwm.main2 > 0 || pwm.led3 > 0 || pwm.led4 > 0);
}

int anduril_is_light_off(avr_t* avr) {
    pwm_state_t pwm = anduril_get_pwm(avr);
    return (pwm.main2 == 0 && pwm.led3 == 0 && pwm.led4 == 0);
}

uint32_t anduril_get_total_output(avr_t* avr) {
    pwm_state_t pwm = anduril_get_pwm(avr);
    return (uint32_t)pwm.main2 + pwm.led3 + pwm.led4;
}

// =============================================================================
// Test Assertions
// =============================================================================

void assert_light_on(avr_t* avr) {
    if (!anduril_is_light_on(avr)) {
        pwm_state_t pwm = anduril_get_pwm(avr);
        char msg[128];
        snprintf(msg, sizeof(msg), "Expected light ON, got main2=%d led3=%d led4=%d",
                 pwm.main2, pwm.led3, pwm.led4);
        TEST_FAIL(msg);
    }
}

void assert_light_off(avr_t* avr) {
    if (!anduril_is_light_off(avr)) {
        pwm_state_t pwm = anduril_get_pwm(avr);
        char msg[128];
        snprintf(msg, sizeof(msg), "Expected light OFF, got main2=%d led3=%d led4=%d",
                 pwm.main2, pwm.led3, pwm.led4);
        TEST_FAIL(msg);
    }
}

void assert_pwm_range(avr_t* avr, uint16_t min_total, uint16_t max_total) {
    uint32_t total = anduril_get_total_output(avr);
    if (total < min_total || total > max_total) {
        char msg[128];
        snprintf(msg, sizeof(msg), "PWM total %u outside range [%u, %u]",
                 (unsigned)total, min_total, max_total);
        TEST_FAIL(msg);
    }
}

void assert_main2_range(avr_t* avr, uint16_t min, uint16_t max) {
    pwm_state_t pwm = anduril_get_pwm(avr);
    if (pwm.main2 < min || pwm.main2 > max) {
        char msg[128];
        snprintf(msg, sizeof(msg), "main2=%u outside range [%u, %u]",
                 pwm.main2, min, max);
        TEST_FAIL(msg);
    }
}

void assert_led3_range(avr_t* avr, uint16_t min, uint16_t max) {
    pwm_state_t pwm = anduril_get_pwm(avr);
    if (pwm.led3 < min || pwm.led3 > max) {
        char msg[128];
        snprintf(msg, sizeof(msg), "led3=%u outside range [%u, %u]",
                 pwm.led3, min, max);
        TEST_FAIL(msg);
    }
}

void assert_led4_range(avr_t* avr, uint16_t min, uint16_t max) {
    pwm_state_t pwm = anduril_get_pwm(avr);
    if (pwm.led4 < min || pwm.led4 > max) {
        char msg[128];
        snprintf(msg, sizeof(msg), "led4=%u outside range [%u, %u]",
                 pwm.led4, min, max);
        TEST_FAIL(msg);
    }
}

// =============================================================================
// Test Suite Helpers
// =============================================================================

int anduril_test_summary(void) {
    printf("\n========================================\n");
    printf("Test Results: %d passed, %d failed\n",
           test_ctx.passed, test_ctx.failed);
    printf("========================================\n");

    return test_ctx.failed > 0 ? 1 : 0;
}

void anduril_print_pwm(avr_t* avr) {
    pwm_state_t pwm = anduril_get_pwm(avr);
    printf("PWM: main2=%5u  led3=%5u  led4=%5u  (total=%u)\n",
           pwm.main2, pwm.led3, pwm.led4,
           (unsigned)(pwm.main2 + pwm.led3 + pwm.led4));
}
