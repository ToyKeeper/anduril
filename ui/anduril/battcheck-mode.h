// battcheck-mode.h: Battery check mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

uint8_t battcheck_state(Event event, uint16_t arg);

#if defined(USE_VOLTAGE_CORRECTION) || defined(USE_POST_OFF_VOLTAGE) || defined(USE_AUX_THRESHOLD_CONFIG)
void voltage_config_save(uint8_t step, uint8_t value);
uint8_t voltage_config_state(Event event, uint16_t arg);
#endif

typedef enum {
    voltage_cfg_zero = 0,
    #ifdef USE_VOLTAGE_CORRECTION
        voltage_correction_config_step,
    #endif
    #ifdef USE_POST_OFF_VOLTAGE
        post_off_voltage_config_step,
    #endif
    #if defined(USE_AUX_THRESHOLD_CONFIG)
        button_led_low_ramp_level_step,
        button_led_high_ramp_level_step,
    #endif
    voltage_config_num_steps
} voltage_config_steps_e;

