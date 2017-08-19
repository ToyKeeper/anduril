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

#define FSM_EMISAR_D4_LAYOUT
#define USE_LVP
#define USE_DEBUG_BLINK
#define USE_DELAY_MS
#include "spaghetti-monster.h"

// ../../bin/level_calc.py 2 7 7135 3 0.25 150 FET 1 10 1500
uint8_t pwm1_modes[] = { 3, 27, 130, 255, 255, 255,   0, };
uint8_t pwm2_modes[] = { 0,  0,  0,   12,  62, 141, 255, };

uint8_t off_state(EventPtr event, uint16_t arg);
uint8_t steady_state(EventPtr event, uint16_t arg);
uint8_t party_strobe_state(EventPtr event, uint16_t arg);

uint8_t current_mode = 0;

void set_mode(uint8_t mode) {
    PWM1_LVL = pwm1_modes[mode];
    PWM2_LVL = pwm2_modes[mode];
}

uint8_t off_state(EventPtr event, uint16_t arg) {
    // turn emitter off when entering state
    if (event == EV_enter_state) {
        PWM1_LVL = 0;
        PWM2_LVL = 0;
        return 0;
    }
    // 1 click: regular mode
    else if (event == EV_1click) {
        set_state(steady_state, current_mode);
        return 0;
    }
    // 1 click (before timeout): go to memorized level, but allow abort for double click
    else if (event == EV_click1_release) {
        set_mode(current_mode);
    }
    // 2 clicks: highest mode
    else if (event == EV_2clicks) {
        set_state(steady_state, sizeof(pwm1_modes)-1);
        return 0;
    }
    // 3 clicks: strobe mode
    else if (event == EV_3clicks) {
        set_state(party_strobe_state, 0);
        return 0;
    }
    // hold (initially): go to lowest level, but allow abort for regular click
    else if (event == EV_click1_press) {
        set_mode(0);
    }
    // hold: go to lowest level
    else if (event == EV_click1_hold) {
        set_state(steady_state, 0);
    }
    return 1;
}

uint8_t steady_state(EventPtr event, uint16_t arg) {
    //static volatile uint8_t current_mode = 0;
    // turn LED on when we first enter the mode
    if (event == EV_enter_state) {
        current_mode = arg;
        set_mode(arg);
        return 0;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return 0;
    }
    // 2 clicks: go to strobe modes
    else if (event == EV_2clicks) {
        set_state(party_strobe_state, 2);
        return 0;
    }
    // hold: change brightness
    else if (event == EV_click1_hold) {
        if ((arg % HOLD_TIMEOUT) == 0) {
            current_mode = (current_mode+1) % sizeof(pwm1_modes);
            set_mode(current_mode);
        }
        return 0;
    }
    return 1;
}

uint8_t party_strobe_state(EventPtr event, uint16_t arg) {
    static volatile uint8_t frames = 0;
    static volatile uint8_t between = 0;
    if (event == EV_enter_state) {
        between = arg;
        frames = 0;
        return 0;
    }
    // strobe the emitter
    else if (event == EV_tick) {
        if (frames == 0) {
            PWM1_LVL = 255;
            PWM2_LVL = 0;
            delay_ms(1);
            PWM1_LVL = 0;
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
        set_state(steady_state, 1);
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
    // FIXME: do something
}

void setup() {
    debug_blink(2);

    push_state(off_state, 0);
}
