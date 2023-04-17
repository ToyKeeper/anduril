// tempcheck-mode.c: Temperature check mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "tempcheck-mode.h"

uint8_t tempcheck_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: next blinky mode
    else if (event == EV_2clicks) {
        #if defined(USE_BEACON_MODE)
        set_state(beacon_state, 0);
        #elif defined(USE_SOS_MODE) && defined(USE_SOS_MODE_IN_BLINKY_GROUP)
        set_state(sos_state, 0);
        #elif defined(USE_BATTCHECK)
        set_state(battcheck_state, 0);
        #endif
        return MISCHIEF_MANAGED;
    }
    // 7H: thermal config mode
    else if (event == EV_click7_hold) {
        push_state(thermal_config_state, 0);
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}

void thermal_config_save(uint8_t step, uint8_t value) {
    if (value) {
        // item 1: calibrate room temperature
        if (step == 1) {
            int8_t rawtemp = temperature - cfg.therm_cal_offset;
            cfg.therm_cal_offset = value - rawtemp;
            adc_reset = 2;  // invalidate all recent temperature data
        }

        // item 2: set maximum heat limit
        else {
            cfg.therm_ceil = 30 + value - 1;
        }
    }

    if (cfg.therm_ceil > MAX_THERM_CEIL) cfg.therm_ceil = MAX_THERM_CEIL;
}

uint8_t thermal_config_state(Event event, uint16_t arg) {
    return config_state_base(event, arg,
                             2, thermal_config_save);
}

