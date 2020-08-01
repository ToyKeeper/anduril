/*
 * load-save-config.c: Load/save/eeprom functions for Anduril.
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

#ifndef LOAD_SAVE_CONFIG_C
#define LOAD_SAVE_CONFIG_C

#include "load-save-config-fsm.h"
#include "load-save-config.h"

void load_config() {
    if (load_eeprom()) {
        ramp_style = eeprom[ramp_style_e];
        #ifdef USE_RAMP_CONFIG
        ramp_floors[0] = eeprom[ramp_smooth_floor_e];
        ramp_ceils[0] = eeprom[ramp_smooth_ceil_e];
        ramp_floors[1] = eeprom[ramp_discrete_floor_e];
        ramp_ceils[1] = eeprom[ramp_discrete_ceil_e];
        ramp_stepss[1] = eeprom[ramp_discrete_steps_e];
        #endif
        #ifdef USE_MANUAL_MEMORY
        manual_memory = eeprom[manual_memory_e];
        #endif
        #ifdef USE_TINT_RAMPING
        tint = eeprom[tint_e];
        #endif
        #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        strobe_type = eeprom[strobe_type_e];  // TODO: move this to eeprom_wl?
        strobe_delays[0] = eeprom[strobe_delays_0_e];
        strobe_delays[1] = eeprom[strobe_delays_1_e];
        #endif
        #ifdef USE_BIKE_FLASHER_MODE
        bike_flasher_brightness = eeprom[bike_flasher_brightness_e];
        #endif
        #ifdef USE_BEACON_MODE
        beacon_seconds = eeprom[beacon_seconds_e];
        #endif
        #ifdef USE_SIMPLE_UI
        simple_ui_active = eeprom[simple_ui_active_e];
        ramp_floors[2] = eeprom[simple_ui_floor_e];
        ramp_ceils[2] = eeprom[simple_ui_ceil_e];
        ramp_stepss[2] = eeprom[simple_ui_steps_e];
        #endif
        #ifdef USE_THERMAL_REGULATION
        therm_ceil = eeprom[therm_ceil_e];
        therm_cal_offset = eeprom[therm_cal_offset_e];
        #endif
        #ifdef USE_INDICATOR_LED
        indicator_led_mode = eeprom[indicator_led_mode_e];
        #endif
        #ifdef USE_AUX_RGB_LEDS
        rgb_led_off_mode = eeprom[rgb_led_off_mode_e];
        rgb_led_lockout_mode = eeprom[rgb_led_lockout_mode_e];
        #endif
        #ifdef USE_AUTOLOCK
        autolock_time = eeprom[autolock_time_e];
        #endif
    }
    #ifdef START_AT_MEMORIZED_LEVEL
    if (load_eeprom_wl()) {
        memorized_level = eeprom_wl[0];
    }
    #endif
}

void save_config() {
    eeprom[ramp_style_e] = ramp_style;
    #ifdef USE_RAMP_CONFIG
    eeprom[ramp_smooth_floor_e] = ramp_floors[0];
    eeprom[ramp_smooth_ceil_e] = ramp_ceils[0];
    eeprom[ramp_discrete_floor_e] = ramp_floors[1];
    eeprom[ramp_discrete_ceil_e] = ramp_ceils[1];
    eeprom[ramp_discrete_steps_e] = ramp_stepss[1];
    #endif
    #ifdef USE_MANUAL_MEMORY
    eeprom[manual_memory_e] = manual_memory;
    #endif
    #ifdef USE_TINT_RAMPING
    eeprom[tint_e] = tint;
    #endif
    #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
    eeprom[strobe_type_e] = strobe_type;  // TODO: move this to eeprom_wl?
    eeprom[strobe_delays_0_e] = strobe_delays[0];
    eeprom[strobe_delays_1_e] = strobe_delays[1];
    #endif
    #ifdef USE_BIKE_FLASHER_MODE
    eeprom[bike_flasher_brightness_e] = bike_flasher_brightness;
    #endif
    #ifdef USE_BEACON_MODE
    eeprom[beacon_seconds_e] = beacon_seconds;
    #endif
    #ifdef USE_SIMPLE_UI
    eeprom[simple_ui_active_e] = simple_ui_active;
    eeprom[simple_ui_floor_e] = ramp_floors[2];
    eeprom[simple_ui_ceil_e] = ramp_ceils[2];
    eeprom[simple_ui_steps_e] = ramp_stepss[2];
    #endif
    #ifdef USE_THERMAL_REGULATION
    eeprom[therm_ceil_e] = therm_ceil;
    eeprom[therm_cal_offset_e] = therm_cal_offset;
    #endif
    #ifdef USE_INDICATOR_LED
    eeprom[indicator_led_mode_e] = indicator_led_mode;
    #endif
    #ifdef USE_AUX_RGB_LEDS
    eeprom[rgb_led_off_mode_e] = rgb_led_off_mode;
    eeprom[rgb_led_lockout_mode_e] = rgb_led_lockout_mode;
    #endif
    #ifdef USE_AUTOLOCK
    eeprom[autolock_time_e] = autolock_time;
    #endif

    save_eeprom();
}

#ifdef START_AT_MEMORIZED_LEVEL
void save_config_wl() {
    eeprom_wl[0] = memorized_level;
    save_eeprom_wl();
}
#endif


#endif

