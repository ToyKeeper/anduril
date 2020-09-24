/*
 * factory-reset.c: Factory reset functions for Anduril.
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

#ifndef FACTORY_RESET_C
#define FACTORY_RESET_C

#include "factory-reset.h"

void factory_reset() {
    // display a warning for a few seconds before doing the actual reset,
    // so the user has time to abort if they want
    #define SPLODEY_TIME 2500
    #define SPLODEY_STEPS 64
    #define SPLODEY_TIME_PER_STEP (SPLODEY_TIME/SPLODEY_STEPS)
    uint8_t bright;
    uint8_t reset = 1;
    // wind up to an explosion
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
        #ifdef USE_THERMAL_REGULATION
        // auto-calibrate temperature...  assume current temperature is 21 C
        thermal_config_save(1, 21);
        #endif
        // save all settings to eeprom
        // (assuming they're all at default because we haven't loaded them yet)
        save_config();

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


#endif

