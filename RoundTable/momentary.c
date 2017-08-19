/*
 * Momentary: Very simple example UI for RoundTable.
 * Is intended to be the simplest possible RT e-switch UI.
 * The light is in while the button is held; off otherwise.
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

#define RT_EMISAR_D4_LAYOUT
#include "round-table.c"

volatile uint8_t brightness;

void light_on() {
    PWM1_LVL = brightness;
    PWM2_LVL = 0;
}

void light_off() {
    PWM1_LVL = 0;
    PWM2_LVL = 0;
}

uint8_t momentary_state(EventPtr event, uint16_t arg) {
    switch(event) {

        case EV_press:
            brightness = 255;
            light_on();
            // reset current event queue
            empty_event_sequence();
            return 0;

        case EV_release:
            light_off();
            // reset current event queue
            empty_event_sequence();
            return 0;

        // LVP / low-voltage protection
        case EV_voltage_low:
        case EV_voltage_critical:
            if (brightness > 0) brightness >>= 1;
            else {
                light_off();
                standby_mode();
            }
            return 0;
    }
    return 1;  // event not handled
}

void setup() {
    set_state(momentary_state);
}
