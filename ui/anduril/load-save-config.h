// load-save-config.h: Load/save/eeprom functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// remember stuff even after battery was changed
void load_config();
void save_config();
#ifdef START_AT_MEMORIZED_LEVEL
void save_config_wl();
#endif

// a struct to hold config values
Config cfg = {

    ///// ramp vars

    // smooth vs discrete ramping
    .ramp_style = RAMP_STYLE,  // 0 = smooth, 1 = discrete
    #ifdef USE_2C_STYLE_CONFIG
        // 1 = A1 style, 2 = A2 style
        .ramp_2c_style = DEFAULT_2C_STYLE,
    #endif

    // settings for each ramp (smooth, stepped, simple UI)
    #ifdef USE_RAMP_CONFIG
        .ramp_floors = {
            RAMP_SMOOTH_FLOOR,
            RAMP_DISCRETE_FLOOR,
            #ifdef USE_SIMPLE_UI
                SIMPLE_UI_FLOOR,
            #endif
            },
        .ramp_ceils = {
            RAMP_SMOOTH_CEIL,
            RAMP_DISCRETE_CEIL,
            #ifdef USE_SIMPLE_UI
                SIMPLE_UI_CEIL,
            #endif
            },
        .ramp_stepss = {
            DEFAULT_RAMP_SPEED,
            RAMP_DISCRETE_STEPS,
            #ifdef USE_SIMPLE_UI
                SIMPLE_UI_STEPS,
            #endif
            },
    #endif

    #ifdef USE_SIMPLE_UI
        // whether to enable the simplified interface or not
        .simple_ui_active = SIMPLE_UI_ACTIVE,
        #ifdef USE_2C_STYLE_CONFIG
            // 0 = no turbo, 1 = A1 style, 2 = A2 style
            .ramp_2c_style_simple = DEFAULT_2C_STYLE_SIMPLE,
        #endif
    #endif

    #ifdef USE_RAMP_AFTER_MOON_CONFIG
        .dont_ramp_after_moon = DEFAULT_DONT_RAMP_AFTER_MOON,
    #endif

    #ifdef USE_MANUAL_MEMORY
        .manual_memory = DEFAULT_MANUAL_MEMORY,
        #ifdef USE_MANUAL_MEMORY_TIMER
            .manual_memory_timer = DEFAULT_MANUAL_MEMORY_TIMER,
        #endif
    #endif

    ///// channel modes / color modes

    #if NUM_CHANNEL_MODES > 1
        // current multi-channel mode
        .channel_mode = DEFAULT_CHANNEL_MODE,
        // user can take unwanted modes out of the rotation (bitmask)
        .channel_modes_enabled = CHANNEL_MODES_ENABLED,
        #ifdef USE_MANUAL_MEMORY
            // reset w/ manual memory
            .manual_memory_channel_mode = DEFAULT_CHANNEL_MODE,
        #endif
        #ifdef DEFAULT_BLINK_CHANNEL
            // blink numbers in a specific channel (user configurable)
            .blink_channel = DEFAULT_BLINK_CHANNEL,
        #endif
    #endif
    #ifdef USE_CHANNEL_MODE_ARGS
        // one byte of extra data per channel mode, like for tint value
        .channel_mode_args = { CHANNEL_MODE_ARGS },
        #ifdef USE_MANUAL_MEMORY
            // remember and reset 1 extra parameter per channel mode (like tint)
            .manual_memory_channel_args = { CHANNEL_MODE_ARGS },
        #endif
        #ifdef USE_STEPPED_TINT_RAMPING
            .tint_ramp_style = DEFAULT_TINT_RAMP_STYLE,
        #endif
    #endif

    ///// Smooth animation between steps, and for on/off
    #ifdef USE_SMOOTH_STEPS
        .smooth_steps_style = DEFAULT_SMOOTH_STEPS_STYLE,
    #endif

    ///// strobe / blinky mode settings

    #ifdef USE_STROBE_STATE
        .strobe_type = DEFAULT_STROBE,
        #if (NUM_CHANNEL_MODES > 1) && defined(USE_CHANNEL_PER_STROBE)
            // channel mode saved per strobe-group mode
            #ifdef DEFAULT_STROBE_CHANNELS
                .strobe_channels = { DEFAULT_STROBE_CHANNELS },
            #endif
        #endif
    #endif
    #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        // party / tactical strobe timing
        // party strobe 24 Hz, tactical strobe 10 Hz
        .strobe_delays = { 41, 67 },
    #endif
    #ifdef USE_BIKE_FLASHER_MODE
        .bike_flasher_brightness = DEFAULT_BIKING_LEVEL,
    #endif
    #ifdef USE_BEACON_MODE
        // beacon timing
        .beacon_seconds = 2,
    #endif

    ///// voltage and temperature

    #ifdef USE_VOLTAGE_CORRECTION
        // same 0.05V units as fudge factor,
        // but 7 is neutral, and the expected range is from 1 to 13
        .voltage_correction = 7,
    #endif
    #ifdef USE_THERMAL_REGULATION
        .therm_ceil = DEFAULT_THERM_CEIL,
        .therm_cal_offset = 0,
    #endif

    ///// aux LEDs

    #ifdef USE_INDICATOR_LED
        // bits 2-3 control lockout mode
        // bits 0-1 control "off" mode
        // modes are: 0=off, 1=low, 2=high, 3=blinking (if TICK_DURING_STANDBY enabled)
        .indicator_led_mode = INDICATOR_LED_DEFAULT_MODE,
    #endif
    #ifdef USE_AUX_RGB_LEDS
        .rgb_led_off_mode = RGB_LED_OFF_DEFAULT,
        .rgb_led_lockout_mode = RGB_LED_LOCKOUT_DEFAULT,
        #ifdef USE_POST_OFF_VOLTAGE
            // display voltage readout for a while after turning off?
            .post_off_voltage = DEFAULT_POST_OFF_VOLTAGE_SECONDS,
        #endif
    #endif

    ///// misc other mode settings

    #ifdef USE_AUTOLOCK
        .autolock_time = DEFAULT_AUTOLOCK_TIME,
    #endif
    #ifdef USE_TACTICAL_MODE
        .tactical_levels = { TACTICAL_LEVELS },
    #endif

    ///// hardware config / globals menu

    #ifdef USE_JUMP_START
        .jump_start_level = DEFAULT_JUMP_START_LEVEL,
    #endif

    #if defined(USE_AUX_RGB_LEDS_WHILE_ON) && defined(USE_CONFIGURABLE_RGB_VOLTAGE_LEVELS)
        // config for RGB voltage. We need to check these here rather than setting defaults in `config-default.h` as we only know *after* defaults are loaded if `USE_AUX_RGB_LEDS_WHILE_ON` is set or unset (in `CFG_H`).
        #if (USE_AUX_RGB_LEDS_WHILE_ON + 0) // if USE_AUX_RGB_LEDS_WHILE_ON is an int, passes. If blank (undefined or defined with no value), evaluates to `(+0)` which evaluates to false.
        .use_aux_rgb_leds_while_on = USE_AUX_RGB_LEDS_WHILE_ON,
        #else
        #warning "USE_AUX_RGB_LEDS_WHILE_ON defined but has no value. Setting minimum threshold to default of 25"
        .use_aux_rgb_leds_while_on = 25,
        #endif
        #ifdef USE_AUX_RGB_LEDS_WHILE_ON_INITIAL_MINIMUM_LEVEL
        .use_aux_rgb_leds_while_on_min_level = USE_AUX_RGB_LEDS_WHILE_ON_INITIAL_MINIMUM_LEVEL,
        #else
        .use_aux_rgb_leds_while_on_min_level = 15, // default
        #endif
    #endif

};

