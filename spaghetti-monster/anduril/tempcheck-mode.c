/*
 * tempcheck-mode.c: Temperature check mode for Anduril.
 *
 * Copyright (C) 2017 Selene ToyKeeper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TEMPCHECK_MODE_C
#define TEMPCHECK_MODE_C

#include "tempcheck-mode.h"

uint8_t tempcheck_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_BATTCHECK
    // 2 clicks: battcheck mode
    else if (event == EV_2clicks) {
        set_state(battcheck_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif
    // 7 clicks: thermal config mode
    else if (event == EV_7clicks) {
        push_state(thermal_config_state, 0);
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}

void thermal_config_save() {
    // parse values
    uint8_t val;

    // calibrate room temperature
    val = config_state_values[0];
    if (val) {
        int8_t rawtemp = temperature - therm_cal_offset;
        therm_cal_offset = val - rawtemp;
        reset_thermal_history = 1;  // invalidate all recent temperature data
    }

    val = config_state_values[1];
    if (val) {
        // set maximum heat limit
        therm_ceil = 30 + val - 1;
    }
    if (therm_ceil > MAX_THERM_CEIL) therm_ceil = MAX_THERM_CEIL;
}

uint8_t thermal_config_state(Event event, uint16_t arg) {
    return config_state_base(event, arg,
                             2, thermal_config_save);
}


#endif

