// sim-avr.c: ATtiny1634 emulator using simavr to run actual firmware
// Copyright (C) 2024 Ian Dobbie / ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This runs the ACTUAL compiled .hex firmware through a cycle-accurate
// AVR emulator, hooking into the hardware registers to visualize LED output.
//
// Build: gcc -o sim-avr sim-avr.c sim-render.c -lsimavr -lm
// Run:   ./sim-avr ../hex/anduril.hank-emisar-d4k-3ch.hex

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "sim_avr.h"
#include "sim_elf.h"
#include "sim_hex.h"
#include "sim_gdb.h"
#include "avr_ioport.h"
#include "avr_timer.h"

#include "sim-render.h"

// =============================================================================
// ATtiny1634 Pin Mapping (from hwdef.h)
// =============================================================================
//
// Main2 PWM (Green x2):  PC0 via OCR0A (8-bit)
// LED3 PWM (Red):        PB3 via OCR1A (16-bit)
// LED4 PWM (Blue):       PA6 via OCR1B (16-bit)
//
// Main2 Enable:          PB0
// LED3 Enable:           PA1
// LED4 Enable:           PA0
//
// Button (e-switch):     PA7 (active low)
//
// Aux LEDs:              PA5 (R), PA4 (G), PA3 (B)
// Button LED:            PA2

#define SWITCH_PIN      7   // PA7
#define MAIN2_ENABLE    0   // PB0
#define LED3_ENABLE     1   // PA1
#define LED4_ENABLE     0   // PA0

// Timer/Counter registers for PWM (ATtiny1634 addresses)
// These will be read directly from AVR memory
#define OCR0A_ADDR  0x48  // Timer0 Compare A (8-bit, main2)
#define OCR1AL_ADDR 0x88  // Timer1 Compare A Low (16-bit, led3)
#define OCR1AH_ADDR 0x89  // Timer1 Compare A High
#define OCR1BL_ADDR 0x8A  // Timer1 Compare B Low (16-bit, led4)
#define OCR1BH_ADDR 0x8B  // Timer1 Compare B High
#define ICR1L_ADDR  0x86  // Timer1 Input Capture (PWM TOP)
#define ICR1H_ADDR  0x87

// =============================================================================
// Global State
// =============================================================================

static avr_t *avr = NULL;
static volatile int running = 1;
static volatile int button_pressed = 0;

// LED PWM values (read from emulator)
static volatile uint16_t main2_pwm = 0;  // Green (x2 LEDs)
static volatile uint16_t led3_pwm = 0;   // Red
static volatile uint16_t led4_pwm = 0;   // Blue

// =============================================================================
// Signal Handler
// =============================================================================

static void sig_handler(int sig) {
    (void)sig;
    running = 0;
}

// =============================================================================
// Button Control
// =============================================================================

// Called when button state changes - updates PA7 in the emulator
static void update_button_state(void) {
    // PA7 is the switch pin, active low
    avr_ioport_external_t ext = {
        .name = 'A',
        .mask = (1 << SWITCH_PIN),
        .value = button_pressed ? 0 : (1 << SWITCH_PIN)
    };
    avr_ioctl(avr, AVR_IOCTL_IOPORT_SET_EXTERNAL('A'), &ext);
}

// =============================================================================
// PWM Value Reading
// =============================================================================

// Read current PWM values from emulator registers
static void read_pwm_values(void) {
    // Read 8-bit OCR0A (main2/green)
    main2_pwm = avr->data[OCR0A_ADDR] << 7;  // Scale 8-bit to ~15-bit

    // Read 16-bit OCR1A (led3/red)
    led3_pwm = avr->data[OCR1AL_ADDR] | (avr->data[OCR1AH_ADDR] << 8);

    // Read 16-bit OCR1B (led4/blue)
    led4_pwm = avr->data[OCR1BL_ADDR] | (avr->data[OCR1BH_ADDR] << 8);
}

// =============================================================================
// AVR Emulator Thread
// =============================================================================

static void* avr_thread(void *arg) {
    (void)arg;

    while (running) {
        // Run the emulator for a while
        int state = avr_run(avr);
        if (state == cpu_Done || state == cpu_Crashed) {
            fprintf(stderr, "AVR emulator stopped (state=%d)\n", state);
            running = 0;
            break;
        }
    }

    return NULL;
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char *argv[]) {
    const char *hex_file = "../hex/anduril.hank-emisar-d4k-3ch.hex";

    if (argc > 1) {
        hex_file = argv[1];
    }

    printf("D4K-3ch AVR Emulator\n");
    printf("Loading firmware: %s\n", hex_file);

    // Set up signal handler
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    // Initialize the AVR emulator for ATtiny1634
    avr = avr_make_mcu_by_name("attiny1634");
    if (!avr) {
        fprintf(stderr, "Error: Failed to create AVR instance\n");
        fprintf(stderr, "Note: simavr may not support attiny1634\n");
        fprintf(stderr, "Try: avr_make_mcu_by_name(\"attiny1634\")\n");
        return 1;
    }

    avr_init(avr);
    avr->frequency = 8000000;  // 8 MHz

    // Load the hex file
    uint32_t loadBase = AVR_SEGMENT_OFFSET_FLASH;
    uint32_t loadSize;
    uint8_t *firmware = read_ihex_file(hex_file, &loadSize, &loadBase);
    if (!firmware) {
        fprintf(stderr, "Error: Failed to load hex file: %s\n", hex_file);
        return 1;
    }

    printf("Loaded %d bytes at 0x%04x\n", loadSize, loadBase);

    // Copy firmware to AVR flash
    memcpy(avr->flash + loadBase, firmware, loadSize);
    free(firmware);

    // Reset the AVR
    avr_reset(avr);

    // Initialize terminal rendering
    render_init();
    atexit(render_cleanup);

    // Start AVR emulator thread
    pthread_t avr_tid;
    pthread_create(&avr_tid, NULL, avr_thread, NULL);

    printf("Emulator running. Press 'q' to quit.\n");
    usleep(500000);

    // Main loop - handle input and render
    while (running) {
        // Non-blocking keyboard input
        char c = 0;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            switch (c) {
                case 'q':
                case 'Q':
                    running = 0;
                    break;

                case 'c':  // Click
                    button_pressed = 1;
                    update_button_state();
                    usleep(50000);  // 50ms press
                    button_pressed = 0;
                    update_button_state();
                    break;

                case 'p':  // Press (start hold)
                    button_pressed = 1;
                    update_button_state();
                    break;

                case 'r':  // Release
                    button_pressed = 0;
                    update_button_state();
                    break;

                case '2':  // Double click
                    for (int i = 0; i < 2; i++) {
                        button_pressed = 1;
                        update_button_state();
                        usleep(30000);
                        button_pressed = 0;
                        update_button_state();
                        usleep(100000);
                    }
                    break;

                case '3':  // Triple click
                    for (int i = 0; i < 3; i++) {
                        button_pressed = 1;
                        update_button_state();
                        usleep(30000);
                        button_pressed = 0;
                        update_button_state();
                        usleep(100000);
                    }
                    break;
            }
        }

        // Read PWM values from emulator
        read_pwm_values();

        // Convert to 8-bit RGB for display
        uint8_t r = led3_pwm >> 7;   // Red
        uint8_t g = main2_pwm >> 7;  // Green (already scaled in firmware)
        uint8_t b = led4_pwm >> 7;   // Blue

        // Render display
        render_display(r, g, b,
                       main2_pwm, led3_pwm, led4_pwm,
                       "emulator",
                       "running",
                       0, 150,
                       0);

        // ~30 FPS
        usleep(33000);
    }

    // Wait for AVR thread to finish
    pthread_join(avr_tid, NULL);

    return 0;
}
