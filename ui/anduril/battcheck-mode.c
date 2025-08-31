// battcheck-mode.c: Battery check mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "anduril/battcheck-mode.h"

uint8_t battcheck_state(Event event, uint16_t arg) {
    ////////// Every action below here is blocked in the simple UI //////////
    #ifdef USE_SIMPLE_UI
    if (cfg.simple_ui_active) {
        return EVENT_NOT_HANDLED;
    }
    #endif

    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }

    // 2 clicks: next blinky mode
    else if (event == EV_2clicks) {
        #if defined(USE_THERMAL_REGULATION)
        set_state(tempcheck_state, 0);
        #elif defined(USE_BEACON_MODE)
        set_state(beacon_state, 0);
        #elif defined(USE_SOS_MODE) && defined(USE_SOS_MODE_IN_BLINKY_GROUP)
        set_state(sos_state, 0);
        #endif
        return EVENT_HANDLED;
    }

    #ifdef DEFAULT_BLINK_CHANNEL
    // 3 clicks: next channel mode (specific to number blinky modes)
    else if (event == EV_3clicks) {
        cfg.blink_channel = (cfg.blink_channel + 1) % NUM_CHANNEL_MODES;
        save_config();
        return EVENT_HANDLED;
    }
    #endif  // ifdef DEFAULT_BLINK_CHANNEL

    #ifdef USE_VOLTAGE_CORRECTION
    // 7H: voltage config mode
    else if (event == EV_click7_hold) {
        push_state(voltage_config_state, 0);
        return EVENT_HANDLED;
    }
    #endif

    return EVENT_NOT_HANDLED;
}

#if defined(USE_VOLTAGE_CORRECTION) || defined(USE_POST_OFF_VOLTAGE) || defined(USE_AUX_THRESHOLD_CONFIG)
// the user can adjust the battery measurements... on a scale of 1 to 13
// 1 = subtract 0.30V
// 2 = subtract 0.25V
// ...
// 7 = no effect (add 0V)
// 8 = add 0.05V
// ...
// 13 = add 0.30V
void voltage_config_save(uint8_t step, uint8_t value) {
    switch (step) {
        #if defined(USE_AUX_THRESHOLD_CONFIG)
        case button_led_low_ramp_level_step:
            // 0 clicks = 255 = never turn on
            cfg.button_led_low_ramp_level = value - 1;
            break;
        case button_led_high_ramp_level_step:
            // 0 clicks = 255 = never turn on
            cfg.button_led_high_ramp_level = value - 1;
            break;
        #endif

        #ifdef USE_POST_OFF_VOLTAGE
        case post_off_voltage_config_step:
            cfg.post_off_voltage = value;
            break;
        #endif

        #ifdef USE_VOLTAGE_CORRECTION
        default:
            if (value) cfg.voltage_correction = value;
            break;
        #endif
    }
}

uint8_t voltage_config_state(Event event, uint16_t arg) {
    return config_state_base(event, arg,
                             voltage_config_num_steps - 1,
                             voltage_config_save);
}
#endif  // #ifdef USE_VOLTAGE_CORRECTION

