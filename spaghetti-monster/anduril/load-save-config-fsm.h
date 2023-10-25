// load-save-config-fsm.h: FSM config for eeprom configuration in Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define USE_EEPROM
// load into a custom RAM location instead of FSM's default byte array
#define EEPROM_OVERRIDE

#ifdef USE_SIMPLE_UI
#define NUM_RAMPS 3
#else
#define NUM_RAMPS 2
#endif

// let FSM know this config struct exists
#define USE_CFG

typedef struct Config {

    ///// ramp vars
    uint8_t ramp_style;
    #ifdef USE_2C_STYLE_CONFIG
        uint8_t ramp_2c_style;
    #endif
    #ifdef USE_RAMP_CONFIG
        uint8_t ramp_floors[NUM_RAMPS];
        uint8_t ramp_ceils [NUM_RAMPS];
        uint8_t ramp_stepss[NUM_RAMPS];
    #endif
    #ifdef USE_SIMPLE_UI
        uint8_t simple_ui_active;
        #ifdef USE_2C_STYLE_CONFIG
            uint8_t ramp_2c_style_simple;
        #endif
    #endif
    #ifdef USE_RAMP_AFTER_MOON_CONFIG
        uint8_t dont_ramp_after_moon;
    #endif
    #ifdef USE_MANUAL_MEMORY
        uint8_t manual_memory;
        #ifdef USE_MANUAL_MEMORY_TIMER
            uint8_t manual_memory_timer;
        #endif
    #endif

    ///// channel modes / color modes
    #if NUM_CHANNEL_MODES > 1
        uint8_t channel_mode;
        uint16_t channel_modes_enabled;
        #ifdef USE_MANUAL_MEMORY
            uint8_t manual_memory_channel_mode;
        #endif
        #ifdef DEFAULT_BLINK_CHANNEL
            uint8_t blink_channel;
        #endif
    #endif
    #ifdef USE_CHANNEL_MODE_ARGS
        // this is an array, needs a few bytes
        uint8_t channel_mode_args[NUM_CHANNEL_MODES];
        #ifdef USE_MANUAL_MEMORY
            uint8_t manual_memory_channel_args[NUM_CHANNEL_MODES];
        #endif
        #ifdef USE_STEPPED_TINT_RAMPING
            uint8_t tint_ramp_style;
        #endif
    #endif

    ///// Smooth animation between steps, and for on/off
    #ifdef USE_SMOOTH_STEPS
        uint8_t smooth_steps_style;
    #endif

    ///// strobe / blinky mode settings
    #ifdef USE_STROBE_STATE
        uint8_t strobe_type;
        #if (NUM_CHANNEL_MODES > 1) && defined(USE_CHANNEL_PER_STROBE)
            uint8_t strobe_channels[NUM_STROBES];
        #endif
    #endif
    #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        uint8_t strobe_delays[2];
    #endif
    #ifdef USE_BIKE_FLASHER_MODE
        uint8_t bike_flasher_brightness;
    #endif
    #ifdef USE_BEACON_MODE
        uint8_t beacon_seconds;
    #endif

    ///// voltage and temperature
    #ifdef USE_VOLTAGE_CORRECTION
        uint8_t voltage_correction;
    #endif
    #ifdef USE_THERMAL_REGULATION
        uint8_t therm_ceil;
        int8_t therm_cal_offset;
    #endif

    ///// aux LEDs
    #ifdef USE_INDICATOR_LED
        uint8_t indicator_led_mode;
    #endif
    #ifdef USE_AUX_RGB_LEDS
        uint8_t rgb_led_off_mode;
        uint8_t rgb_led_lockout_mode;
        #ifdef USE_POST_OFF_VOLTAGE
            uint8_t post_off_voltage;
        #endif
    #endif

    ///// misc other mode settings
    #ifdef USE_AUTOLOCK
        uint8_t autolock_time;
    #endif
    #ifdef USE_TACTICAL_MODE
        uint8_t tactical_levels[3];
    #endif

    ///// hardware config / globals menu
    #ifdef USE_JUMP_START
        uint8_t jump_start_level;
    #endif

} Config;

// auto-detect how many eeprom bytes
#define EEPROM_BYTES sizeof(Config)

// declare this so FSM can see it,
// but define its values in a file which loads later
Config cfg;

#ifdef START_AT_MEMORIZED_LEVEL
#define USE_EEPROM_WL
#define EEPROM_WL_BYTES 1
#endif

