// sim-interface.c: Interactive interface for Anduril simulator
// Copyright (C) 2024 ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Provides a text-based protocol over stdin/stdout for external UIs
// to control the simavr-based Anduril emulator.
//
// Protocol: Line-delimited commands and responses
// Commands: INIT, RESET, CLICK, HOLD, PRESS, RELEASE, MULTI, RUN, GETPWM, QUIT
// Responses: OK, ERROR:msg, PWM:main2,led3,led4, BYE
//
// Usage: ./sim-interface
//   Then send commands via stdin, read responses from stdout

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tests/anduril-test.h"

// =============================================================================
// Global State
// =============================================================================

static avr_t* avr = NULL;
static bool initialized = false;
static int8_t current_temperature = 25;  // Track temperature in Celsius
static uint16_t current_voltage_mv = 3700;  // Track voltage in millivolts
static bool debug_mode = false;  // Enable with DEBUG=1 environment variable

// =============================================================================
// Protocol Handlers
// =============================================================================

void handle_init(const char* args) {
    if (initialized) {
        printf("ERROR:Already initialized\n");
        return;
    }

    if (!args || strlen(args) == 0) {
        printf("ERROR:Missing hex file path\n");
        return;
    }

    // Initialize emulator with provided hex file
    avr = anduril_test_init(args);
    if (!avr) {
        printf("ERROR:Failed to initialize emulator\n");
        return;
    }

    initialized = true;
    printf("OK\n");
}

void handle_reset(void) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    anduril_test_reset(avr);
    printf("OK\n");
}

void handle_click(void) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    anduril_click(avr);
    printf("OK\n");
}

void handle_hold(const char* args) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    if (!args || strlen(args) == 0) {
        printf("ERROR:Missing tick count\n");
        return;
    }

    int ticks = atoi(args);
    if (ticks <= 0) {
        printf("ERROR:Invalid tick count\n");
        return;
    }

    anduril_hold(avr, ticks);
    printf("OK\n");
}

void handle_press(void) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    anduril_button_set(avr, 1);
    printf("OK\n");
}

void handle_release(void) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    anduril_button_set(avr, 0);
    printf("OK\n");
}

void handle_multi(const char* args) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    if (!args || strlen(args) == 0) {
        printf("ERROR:Missing click count\n");
        return;
    }

    int count = atoi(args);
    if (count <= 0 || count > 20) {
        printf("ERROR:Invalid click count (1-20)\n");
        return;
    }

    anduril_multi_click(avr, count);
    printf("OK\n");
}

void handle_run(const char* args) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    if (!args || strlen(args) == 0) {
        printf("ERROR:Missing tick count\n");
        return;
    }

    int ticks = atoi(args);
    if (ticks <= 0) {
        printf("ERROR:Invalid tick count\n");
        return;
    }

    anduril_run_ticks(avr, ticks);
    printf("OK\n");
}

void handle_getpwm(void) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    pwm_state_t pwm = anduril_get_pwm(avr);
    printf("PWM:%u,%u,%u,%u,%u,%u\n", pwm.main2, pwm.led3, pwm.led4, pwm.aux_r, pwm.aux_g, pwm.aux_b);

    // Log PWM changes in debug mode
    static uint16_t last_main2 = 0;
    static uint16_t last_led3 = 0;
    static uint16_t last_led4 = 0;
    static uint32_t last_total = 0;

    if (debug_mode) {
        uint32_t total = pwm.main2 + pwm.led3 + pwm.led4;
        if (pwm.main2 != last_main2 || pwm.led3 != last_led3 || pwm.led4 != last_led4) {
            fprintf(stderr, "[DEBUG] PWM: main2=%u led3=%u led4=%u total=%u",
                    pwm.main2, pwm.led3, pwm.led4, total);

            // Detect stepdown (>20% drop)
            if (last_total > 0 && total < (last_total * 80 / 100)) {
                fprintf(stderr, " *** STEPDOWN! (-%u, -%.1f%%) ***",
                        last_total - total,
                        100.0 * (last_total - total) / last_total);
            }
            fprintf(stderr, "\n");

            last_main2 = pwm.main2;
            last_led3 = pwm.led3;
            last_led4 = pwm.led4;
            last_total = total;
        }
    }
}

void handle_setvoltage(const char* args) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    if (!args || strlen(args) == 0) {
        printf("ERROR:Missing voltage in millivolts\n");
        return;
    }

    int millivolts = atoi(args);
    if (millivolts < 2000 || millivolts > 4500) {
        printf("ERROR:Voltage out of range (2000-4500mV)\n");
        return;
    }

    current_voltage_mv = millivolts;
    anduril_set_voltage(avr, millivolts);

    if (debug_mode) {
        fprintf(stderr, "[DEBUG] Voltage set to %dmV\n", millivolts);
    }

    printf("OK\n");
}

void handle_settemp(const char* args) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    if (!args || strlen(args) == 0) {
        printf("ERROR:Missing temperature in Celsius\n");
        return;
    }

    int temp = atoi(args);
    if (temp < -40 || temp > 85) {
        printf("ERROR:Temperature out of range (-40 to 85°C)\n");
        return;
    }

    current_temperature = temp;
    anduril_set_temperature(avr, temp);
    printf("OK\n");
}

void handle_gettemp(void) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    printf("TEMP:%d\n", current_temperature);
}

void handle_geteeprom(const char* args) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    if (!args || strlen(args) == 0) {
        printf("ERROR:Missing offset,length\n");
        return;
    }

    // Parse "offset,length"
    int offset, length;
    if (sscanf(args, "%d,%d", &offset, &length) != 2) {
        printf("ERROR:Invalid format, expected offset,length\n");
        return;
    }

    if (offset < 0 || offset > 255 || length < 1 || offset + length > 256) {
        printf("ERROR:Invalid offset or length\n");
        return;
    }

    // Read EEPROM
    uint8_t buffer[256];
    anduril_eeprom_dump(avr, buffer, offset, length);

    // Format as hex string
    printf("EEPROM:");
    for (int i = 0; i < length; i++) {
        printf("%02X", buffer[i]);
    }
    printf("\n");
}

void handle_dumpeeprom(void) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    // Dump all 256 bytes
    uint8_t buffer[256];
    anduril_eeprom_dump(avr, buffer, 0, 256);

    // Format as hex string
    printf("EEPROM:");
    for (int i = 0; i < 256; i++) {
        printf("%02X", buffer[i]);
    }
    printf("\n");
}

void handle_seteeprom(const char* args) {
    if (!initialized) {
        printf("ERROR:Not initialized\n");
        return;
    }

    if (!args || strlen(args) == 0) {
        printf("ERROR:Missing offset,value\n");
        return;
    }

    // Parse "offset,value"
    int offset, value;
    if (sscanf(args, "%d,%d", &offset, &value) != 2) {
        printf("ERROR:Invalid format, expected offset,value\n");
        return;
    }

    if (offset < 0 || offset > 255) {
        printf("ERROR:Invalid offset (0-255)\n");
        return;
    }

    if (value < 0 || value > 255) {
        printf("ERROR:Invalid value (0-255)\n");
        return;
    }

    anduril_eeprom_write(avr, offset, value);
    printf("OK\n");
}

void handle_quit(void) {
    printf("BYE\n");
    fflush(stdout);

    if (avr) {
        anduril_test_cleanup(avr);
    }

    exit(0);
}

// =============================================================================
// Command Parser
// =============================================================================

void process_command(char* line) {
    // Remove trailing newline
    size_t len = strlen(line);
    if (len > 0 && line[len-1] == '\n') {
        line[len-1] = '\0';
    }

    // Skip empty lines
    if (strlen(line) == 0) {
        return;
    }

    // Split command and args at ':'
    char* cmd = line;
    char* args = strchr(line, ':');
    if (args) {
        *args = '\0';  // Null-terminate command
        args++;        // Point to arguments
    }

    // Dispatch to handler
    if (strcmp(cmd, "INIT") == 0) {
        handle_init(args);
    }
    else if (strcmp(cmd, "RESET") == 0) {
        handle_reset();
    }
    else if (strcmp(cmd, "CLICK") == 0) {
        handle_click();
    }
    else if (strcmp(cmd, "HOLD") == 0) {
        handle_hold(args);
    }
    else if (strcmp(cmd, "PRESS") == 0) {
        handle_press();
    }
    else if (strcmp(cmd, "RELEASE") == 0) {
        handle_release();
    }
    else if (strcmp(cmd, "MULTI") == 0) {
        handle_multi(args);
    }
    else if (strcmp(cmd, "RUN") == 0) {
        handle_run(args);
    }
    else if (strcmp(cmd, "GETPWM") == 0) {
        handle_getpwm();
    }
    else if (strcmp(cmd, "SETVOLTAGE") == 0) {
        handle_setvoltage(args);
    }
    else if (strcmp(cmd, "SETTEMP") == 0) {
        handle_settemp(args);
    }
    else if (strcmp(cmd, "GETTEMP") == 0) {
        handle_gettemp();
    }
    else if (strcmp(cmd, "GETEEPROM") == 0) {
        handle_geteeprom(args);
    }
    else if (strcmp(cmd, "DUMPEEPROM") == 0) {
        handle_dumpeeprom();
    }
    else if (strcmp(cmd, "SETEEPROM") == 0) {
        handle_seteeprom(args);
    }
    else if (strcmp(cmd, "DEBUG_ADC") == 0) {
        if (!initialized) {
            printf("ERROR:Not initialized\n");
        } else {
            // ATtiny1634: I/O space starts at 0x20 in data memory
            // ADCL=I/O 0x00, ADCH=I/O 0x01, ADCSRA=I/O 0x03, ADMUX=I/O 0x04
            uint8_t adcl = avr->data[0x20];      // I/O 0x00
            uint8_t adch = avr->data[0x21];      // I/O 0x01
            uint8_t adcsra = avr->data[0x23];    // I/O 0x03
            uint8_t admux = avr->data[0x24];     // I/O 0x04
            uint16_t adc_raw = adcl | (adch << 8);
            // Firmware uses left-adjusted mode: 10-bit result is in bits [15:6]
            uint16_t adc_result = adc_raw >> 6;
            printf("ADC_DEBUG:ADMUX=0x%02X ADCSRA=0x%02X ADC=%u (raw=0x%04X)\n",
                   admux, adcsra, adc_result, adc_raw);
        }
    }
    else if (strcmp(cmd, "QUIT") == 0) {
        handle_quit();
    }
    else {
        printf("ERROR:Unknown command: %s\n", cmd);
    }

    fflush(stdout);
}

// =============================================================================
// Main Loop
// =============================================================================

int main(int argc, char* argv[]) {
    char line[1024];

    // Check for debug mode via environment variable
    char* debug_env = getenv("DEBUG");
    if (debug_env && atoi(debug_env) != 0) {
        debug_mode = true;
        fprintf(stderr, "[DEBUG] Debug mode enabled\n");
    }

    // Print startup banner to stderr (so it doesn't interfere with protocol)
    fprintf(stderr, "Anduril Simulator Interface v1.0\n");
    fprintf(stderr, "Ready for commands. Type QUIT to exit.\n");
    fprintf(stderr, "Protocol: COMMAND[:args]\\n\n");

    // Main command loop
    while (fgets(line, sizeof(line), stdin) != NULL) {
        process_command(line);
    }

    // EOF reached (pipe closed)
    if (avr) {
        anduril_test_cleanup(avr);
    }

    return 0;
}
