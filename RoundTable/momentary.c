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
#define USE_LVP
#define USE_DEBUG_BLINK
#define OWN_DELAY
#define USE_DELAY_MS
#include "round-table.c"
#include "tk-delay.h"

volatile uint8_t brightness;

void light_on() {
    PWM1_LVL = brightness;
    PWM2_LVL = 0;
}

void light_off() {
    PWM1_LVL = 0;
    PWM2_LVL = 0;
}

//State momentary_state {
uint8_t momentary_state(EventPtr event, uint16_t arg) {

    if (event == EV_click1_press) {
        brightness = 255;
        light_on();
        // reset current event queue
        empty_event_sequence();
        return 0;
    }

    else if (event == EV_release) {
        light_off();
        // reset current event queue
        empty_event_sequence();
        return 0;
    }

    /*
    // LVP / low-voltage protection
    //else if ((event == EV_voltage_low)  ||  (event == EV_voltage_critical)) {
    else if (event == EV_voltage_low) {
        if (brightness > 0) brightness >>= 1;
        else {
            light_off();
            standby_mode();
        }
        return 0;
    }
    */

    // event not handled
    return 1;
}

// LVP / low-voltage protection
void low_voltage() {
    if (brightness > 0) brightness >>= 1;
    else {
        light_off();
        standby_mode();
    }
}

void setup() {
    //debug_blink(1);
    /*
    brightness = 255;
    light_on();
    delay_ms(10);
    light_off();
    */

    push_state(momentary_state);
}
