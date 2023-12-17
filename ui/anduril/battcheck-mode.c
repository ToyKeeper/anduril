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

#ifdef USE_VOLTAGE_CORRECTION
// the user can adjust the battery measurements... on a scale of 1 to 13
// 1 = subtract 0.30V
// 2 = subtract 0.25V
// ...
// 7 = no effect (add 0V)
// 8 = add 0.05V
// ...
// 13 = add 0.30V
void voltage_config_save(uint8_t step, uint8_t value) {
    #if defined(USE_AUX_RGB_LEDS_WHILE_ON) && defined(USE_CONFIGURABLE_RGB_VOLTAGE_LEVELS)
      if (use_aux_rgb_leds_while_on_config_step == step) cfg.use_aux_rgb_leds_while_on = value;
      else if (use_aux_rgb_leds_while_on_min_level_step == step) cfg.use_aux_rgb_leds_while_on_min_level = value;
      else
    #endif
    #ifdef USE_POST_OFF_VOLTAGE
      if (post_off_voltage_config_step == step) cfg.post_off_voltage = value;
      else
    #endif
    #ifdef USE_VOLTAGE_CORRECTION
      if (value) cfg.voltage_correction = value;
    #endif
}

uint8_t voltage_config_state(Event event, uint16_t arg) {
    return config_state_base(event, arg,
                             (voltage_config_num_steps - 1),
                             voltage_config_save);
}
#endif  // #ifdef USE_VOLTAGE_CORRECTION

