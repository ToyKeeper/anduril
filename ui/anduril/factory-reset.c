// factory-reset.c: Factory reset functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/factory-reset.h"

// allows setting channel mode per animation stage,
// so it can ramp up in red then explode in white (as one example)

void factory_reset() {
    // display a warning for a few seconds before doing the actual reset,
    // so the user has time to abort if they want
    #define SPLODEY_TIME 2500
    #define SPLODEY_STEPS 64
    #define SPLODEY_TIME_PER_STEP (SPLODEY_TIME/SPLODEY_STEPS)
    uint8_t bright;
    uint8_t reset = 1;
    // wind up to an explosion
    #ifdef FACTORY_RESET_WARN_CHANNEL
    set_channel_mode(FACTORY_RESET_WARN_CHANNEL);
    #endif
    for (bright=0; bright<SPLODEY_STEPS; bright++) {
        set_level(bright);
        nice_delay_ms(SPLODEY_TIME_PER_STEP/2);
        set_level(bright>>1);
        nice_delay_ms(SPLODEY_TIME_PER_STEP/2);
        if (! button_is_pressed()) {
            reset = 0;
            break;
        }
    }
    // explode, if button pressed long enough
    if (reset) {
        #if defined(FACTORY_RESET_WARN_CHANNEL) && defined(DEFAULT_CHANNEL_MODE)
        // return to default channel before saving
        set_channel_mode(DEFAULT_CHANNEL_MODE);
        #endif

        // auto-calibrate temperature
        // AVR 1-Series has factory calibrated thermal sensor, always remove the offset on reset
        #if defined(USE_THERMAL_REGULATION) && defined(AVRXMEGA3)
        // this will cancel out the offset
        thermal_config_save(1, temperature - cfg.therm_cal_offset);
        #elif defined(USE_THERMAL_REGULATION) && defined(USE_THERM_AUTOCALIBRATE)
        // assume current temperature is 21 C
        thermal_config_save(1, 21);
        #endif

        // save all settings to eeprom
        // (assuming they're all at default because we haven't loaded them yet)
        save_config();

        // explosion animation
        #ifdef FACTORY_RESET_SUCCESS_CHANNEL
        set_channel_mode(FACTORY_RESET_SUCCESS_CHANNEL);
        #endif
        bright = MAX_LEVEL;
        for (; bright > 0; bright--) {
            set_level(bright);
            nice_delay_ms(SPLODEY_TIME_PER_STEP/8);
        }
    }
    // explosion cancelled, fade away
    else {
        for (; bright > 0; bright--) {
            set_level(bright);
            nice_delay_ms(SPLODEY_TIME_PER_STEP/3);
        }
    }
}

