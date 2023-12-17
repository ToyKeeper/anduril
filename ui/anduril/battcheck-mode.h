// battcheck-mode.h: Battery check mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

uint8_t battcheck_state(Event event, uint16_t arg);

#ifdef USE_VOLTAGE_CORRECTION
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
    #if defined(USE_AUX_RGB_LEDS_WHILE_ON) && defined(USE_CONFIGURABLE_RGB_VOLTAGE_LEVELS)
      use_aux_rgb_leds_while_on_config_step,
      use_aux_rgb_leds_while_on_min_level_step,
    #endif
    voltage_config_num_steps
} voltage_config_steps_e;

