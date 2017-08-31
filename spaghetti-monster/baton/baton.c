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

#define FSM_EMISAR_D4_DRIVER
#define USE_LVP
#define USE_THERMAL_REGULATION
#define DEFAULT_THERM_CEIL 45
#define USE_DEBUG_BLINK
#define USE_DELAY_MS
#define USE_DELAY_4MS
#define USE_DELAY_ZERO
#include "spaghetti-monster.h"

// moon + ../../bin/level_calc.py 2 6 7135 18 10 150 FET 1 10 1500
uint8_t pwm1_modes[] = { 3, 18, 110, 255, 255, 255,   0, };
uint8_t pwm2_modes[] = { 0,  0,   0,   9,  58, 138, 255, };
#define MAX_LEVEL (sizeof(pwm1_modes)-1)

// FSM states
uint8_t off_state(EventPtr event, uint16_t arg);
uint8_t steady_state(EventPtr event, uint16_t arg);
uint8_t party_strobe_state(EventPtr event, uint16_t arg);

// brightness control
uint8_t memorized_level = 1;
uint8_t actual_level = 0;
#ifdef USE_THERMAL_REGULATION
uint8_t target_level = 0;
#endif

void set_level(uint8_t lvl) {
    actual_level = lvl;
    PWM1_LVL = pwm1_modes[lvl];
    PWM2_LVL = pwm2_modes[lvl];
}

uint8_t off_state(EventPtr event, uint16_t arg) {
    // turn emitter off when entering state
    if (event == EV_enter_state) {
        PWM1_LVL = 0;
        PWM2_LVL = 0;
        // sleep while off  (lower power use)
        //empty_event_sequence();  // just in case (but shouldn't be needed)
        standby_mode();
        return 0;
    }
    // hold (initially): go to lowest level, but allow abort for regular click
    else if (event == EV_click1_press) {
        set_level(0);
        return 0;
    }
    // 1 click (before timeout): go to memorized level, but allow abort for double click
    else if (event == EV_click1_release) {
        set_level(memorized_level);
        return 0;
    }
    // 1 click: regular mode
    else if (event == EV_1click) {
        set_state(steady_state, memorized_level);
        return 0;
    }
    // 2 clicks: highest mode
    else if (event == EV_2clicks) {
        set_state(steady_state, MAX_LEVEL);
        return 0;
    }
    // 3 clicks: strobe mode
    else if (event == EV_3clicks) {
        set_state(party_strobe_state, 255);
        return 0;
    }
    // hold: go to lowest level
    else if (event == EV_click1_hold) {
        set_state(steady_state, 0);
        return 0;
    }
    return 1;
}

uint8_t steady_state(EventPtr event, uint16_t arg) {
    // turn LED on when we first enter the mode
    if (event == EV_enter_state) {
        // remember this level, unless it's moon or turbo
        if ((arg > 0) && (arg < MAX_LEVEL))
            memorized_level = arg;
        // use the requested level even if not memorized
        #ifdef USE_THERMAL_REGULATION
        target_level = arg;
        #endif
        set_level(arg);
        return 0;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return 0;
    }
    // 2 clicks: go to/from highest level
    else if (event == EV_2clicks) {
        if (actual_level < MAX_LEVEL) {
            memorized_level = actual_level;  // in case we're on moon
            #ifdef USE_THERMAL_REGULATION
            target_level = MAX_LEVEL;
            #endif
            set_level(MAX_LEVEL);
        }
        else {
            #ifdef USE_THERMAL_REGULATION
            target_level = memorized_level;
            #endif
            set_level(memorized_level);
        }
        return 0;
    }
    // 3 clicks: go to strobe modes
    else if (event == EV_3clicks) {
        set_state(party_strobe_state, 0xff);
        return 0;
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
        return 0;
    }
    #ifdef USE_THERMAL_REGULATION
    // overheating: drop by 1 level
    else if (event == EV_temperature_high) {
        if (actual_level > 1) {
            set_level(actual_level - 1);
        }
        return 0;
    }
    // underheating: increase by 1 level if we're lower than the target
    else if (event == EV_temperature_low) {
        if (actual_level < target_level) {
            set_level(actual_level + 1);
        }
        return 0;
    }
    #endif
    return 1;
}

uint8_t party_strobe_state(EventPtr event, uint16_t arg) {
    static volatile uint8_t frames = 0;
    static volatile uint8_t between = 2;
    if (event == EV_enter_state) {
        if (arg < 64) between = arg;
        frames = 0;
        return 0;
    }
    // tick: strobe the emitter
    else if (event == EV_tick) {
        if (frames == 0) {
            PWM1_LVL = 0;
            PWM2_LVL = 255;
            if (between < 3) delay_zero();
            else delay_ms(1);
            PWM2_LVL = 0;
        }
        //frames = (frames + 1) % between;
        frames++;
        if (frames > between) frames = 0;
        return 0;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return 0;
    }
    // 2 clicks: go back to regular modes
    else if (event == EV_2clicks) {
        set_state(steady_state, memorized_level);
        return 0;
    }
    // hold: change speed
    else if (event == EV_click1_hold) {
        if ((arg % HOLD_TIMEOUT) == 0) {
            between = (between+1)%6;
            frames = 0;
        }
        return 0;
    }
    return 1;
}

void low_voltage() {
    // "step down" from strobe to level 2
    if (current_state == party_strobe_state) {
        set_state(steady_state, 1);
    }
    // in normal mode, step down by one level or turn off
    else if (current_state == steady_state) {
        if (actual_level > 0) {
            set_level(actual_level - 1);
        }
        else {
            set_state(off_state, 0);
        }
    }
}

void setup() {
    debug_blink(2);

    push_state(off_state, 0);
}

void loop() { }
