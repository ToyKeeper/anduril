/*
 * Baton: Olight Baton-like UI for SpaghettiMonster.
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

#include "hwdef-Emisar_D4.h"
#define USE_LVP
#define USE_THERMAL_REGULATION
#define USE_DELAY_MS
#include "spaghetti-monster.h"

// FSM states
uint8_t off_state(Event event, uint16_t arg);
uint8_t steady_state(Event event, uint16_t arg);
uint8_t lockout_state(Event event, uint16_t arg);

// brightness control
uint8_t memorized_level = 1;
uint8_t actual_level = 0;
#ifdef USE_THERMAL_REGULATION
uint8_t target_level = 0;
#endif

// moon + ../../bin/level_calc.py 2 6 7135 18 10 150 FET 1 10 1500
uint8_t pwm1_levels[] = { 3, 18, 110, 255, 255, 255,   0, };
uint8_t pwm2_levels[] = { 0,  0,   0,   9,  58, 138, 255, };
#define MAX_LEVEL (sizeof(pwm1_levels)-1)

// set LED brightness
void set_level(uint8_t lvl) {
    actual_level = lvl;
    PWM1_LVL = pwm1_levels[lvl];
    PWM2_LVL = pwm2_levels[lvl];
}

uint8_t off_state(Event event, uint16_t arg) {
    // turn emitter off when entering state
    if (event == EV_enter_state) {
        go_to_standby = 1; // sleep while off  (lower power use)
        return EVENT_HANDLED;
    }
    // hold (initially): go to lowest level, but allow abort for regular click
    else if (event == EV_click1_press) {
        set_level(0);
        return EVENT_HANDLED;
    }
    // hold (longer): go to lowest level
    else if (event == EV_click1_hold) {
        set_state(steady_state, 0);
        return EVENT_HANDLED;
    }
    // 1 click (before timeout): go to memorized level, but allow abort for double click
    else if (event == EV_click1_release) {
        set_level(memorized_level);
        return EVENT_HANDLED;
    }
    // 1 click: regular mode
    else if (event == EV_1click) {
        set_state(steady_state, memorized_level);
        return EVENT_HANDLED;
    }
    // 2 clicks: highest mode
    else if (event == EV_2clicks) {
        set_state(steady_state, MAX_LEVEL);
        return EVENT_HANDLED;
    }
    // 4 clicks: soft lockout
    else if (event == EV_4clicks) {
        set_state(lockout_state, 0);
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}

uint8_t steady_state(Event event, uint16_t arg) {
    // turn LED on when we first enter the mode
    if (event == EV_enter_state) {
        // remember this level, unless it's moon or turbo
        if ((arg > 0) && (arg < MAX_LEVEL)) memorized_level = arg;
        // use the requested level even if not memorized
        #ifdef USE_THERMAL_REGULATION
        target_level = arg;
        #endif
        set_level(arg);
        return EVENT_HANDLED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    // 2 clicks: go to/from highest level
    else if (event == EV_2clicks) {
        if (actual_level < MAX_LEVEL) {  // go to turbo
            memorized_level = actual_level;  // in case we're on moon
            #ifdef USE_THERMAL_REGULATION
            target_level = MAX_LEVEL;
            #endif
            set_level(MAX_LEVEL);
        }
        else {  // return from turbo
            #ifdef USE_THERMAL_REGULATION
            target_level = memorized_level;
            #endif
            set_level(memorized_level);
        }
        return EVENT_HANDLED;
    }
    // hold: change brightness
    else if (event == EV_click1_hold) {
        if ((arg % HOLD_TIMEOUT) == 0) {
            memorized_level = (actual_level+1) % (MAX_LEVEL+1);
            #ifdef USE_THERMAL_REGULATION
            target_level = memorized_level;
            #endif
            set_level(memorized_level);
        }
        return EVENT_HANDLED;
    }
    #ifdef USE_THERMAL_REGULATION
    // overheating: drop by 1 level
    else if (event == EV_temperature_high) {
        if (actual_level > 1) { set_level(actual_level - 1); }
        return EVENT_HANDLED;
    }
    // underheating: increase by 1 level if we're lower than the target
    else if (event == EV_temperature_low) {
        if (actual_level < target_level) { set_level(actual_level + 1); }
        return EVENT_HANDLED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}

uint8_t lockout_state(Event event, uint16_t arg) {
    // stay asleep while locked
    if (event == EV_tick) {
        PWM1_LVL = 0;  PWM2_LVL = 0;  // make sure emitters are off
        // sleep 1 second after user stops pressing buttons
        if (arg > TICKS_PER_SECOND) { go_to_standby = 1; }
        return EVENT_HANDLED;
    }
    // 4 clicks: exit, and turn on at "low" level
    else if (event == EV_4clicks) {
        set_state(steady_state, 1);
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}

void low_voltage() {
    // step down by one level or turn off
    if (actual_level > 0) {
        set_level(actual_level - 1);
        #ifdef USE_THERMAL_REGULATION
        target_level = actual_level;  // don't let low temperature override LVP
        #endif
    }
    else {
        set_state(off_state, 0);
    }
}

void setup() {
    // blink when power is connected
    set_level(MAX_LEVEL/2);
    delay_ms(10);
    set_level(0);

    push_state(off_state, 0);
}

void loop() {
}
