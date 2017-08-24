/*
 * Ramping-UI: Ramping UI for SpaghettiMonster.
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
#define USE_THERMAL_REGULATION
#define DEFAULT_THERM_CEIL 32
#define USE_DEBUG_BLINK
#define USE_DELAY_MS
#define USE_DELAY_ZERO
#define USE_RAMPING
#define RAMP_LENGTH 150
#include "spaghetti-monster.h"

// FSM states
uint8_t off_state(EventPtr event, uint16_t arg);
uint8_t steady_state(EventPtr event, uint16_t arg);
uint8_t party_strobe_state(EventPtr event, uint16_t arg);

// brightness control
uint8_t memorized_level = MAX_1x7135;
// smooth vs discrete ramping
uint8_t ramp_step_size = 1;

#ifdef USE_THERMAL_REGULATION
// brightness before thermal step-down
uint8_t target_level = 0;
#endif

uint8_t off_state(EventPtr event, uint16_t arg) {
    // turn emitter off when entering state
    if (event == EV_enter_state) {
        set_level(0);
        // sleep while off  (lower power use)
        //empty_event_sequence();  // just in case (but shouldn't be needed)
        standby_mode();
        return 0;
    }
    // hold (initially): go to lowest level, but allow abort for regular click
    else if (event == EV_click1_press) {
        set_level(1);
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
        // don't start ramping immediately;
        // give the user time to release at moon level
        if (arg >= HOLD_TIMEOUT)
            set_state(steady_state, 1);
        return 0;
    }
    // hold, release quickly: go to lowest level
    else if (event == EV_click1_hold_release) {
        set_state(steady_state, 1);
        return 0;
    }
    // click-release-hold: go to highest level (for ramping down)
    else if (event == EV_click2_hold) {
        set_state(steady_state, MAX_LEVEL);
        return 0;
    }
    return 1;
}

uint8_t steady_state(EventPtr event, uint16_t arg) {
    // turn LED on when we first enter the mode
    if (event == EV_enter_state) {
        // remember this level, unless it's moon or turbo
        if ((arg > 1) && (arg < MAX_LEVEL))
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
    // 4 clicks: toggle smooth vs discrete ramping
    else if (event == EV_4clicks) {
        if (ramp_step_size == 1) ramp_step_size = MAX_LEVEL/6;
        else ramp_step_size = 1;
        set_level(0);
        delay_ms(20);
        set_level(memorized_level);
        return 0;
    }
    // hold: change brightness (brighter)
    else if (event == EV_click1_hold) {
        if (arg % ramp_step_size != 0) {
            return 0;
        }
        // FIXME: make it ramp down instead, if already at max
        if (actual_level + ramp_step_size < MAX_LEVEL)
            memorized_level = actual_level + ramp_step_size;
        else memorized_level = MAX_LEVEL;
        #ifdef USE_THERMAL_REGULATION
        target_level = memorized_level;
        #endif
        // FIXME: only blink once
        if ((memorized_level == MAX_1x7135) || (memorized_level == MAX_LEVEL)) {
            set_level(0);
            delay_ms(7);
        }
        set_level(memorized_level);
        return 0;
    }
    // click-release-hold: change brightness (dimmer)
    else if (event == EV_click2_hold) {
        if (arg % ramp_step_size != 0) {
            return 0;
        }
        // FIXME: make it ramp up instead, if already at min
        if (actual_level > ramp_step_size)
            memorized_level = (actual_level-ramp_step_size);
        else
            memorized_level = 1;
        #ifdef USE_THERMAL_REGULATION
        target_level = memorized_level;
        #endif
        // FIXME: only blink once
        if ((memorized_level == MAX_1x7135) || (memorized_level == 1)) {
            set_level(0);
            delay_ms(7);
        }
        set_level(memorized_level);
        return 0;
    }
    #ifdef USE_THERMAL_REGULATION
    // TODO: test this on a real light
    // overheating: drop by an amount proportional to how far we are above the ceiling
    else if (event == EV_temperature_high) {
        if (actual_level > MAX_LEVEL/4) {
            uint8_t stepdown = actual_level - arg;
            if (stepdown < MAX_LEVEL/4) stepdown = MAX_LEVEL/4;
            set_level(stepdown);
        }
        return 0;
    }
    // underheating: increase slowly if we're lower than the target
    //               (proportional to how low we are)
    else if (event == EV_temperature_low) {
        if (actual_level < target_level) {
            uint8_t stepup = actual_level + (arg>>1);
            if (stepup > target_level) stepup = target_level;
            set_level(stepup);
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
    // "step down" from strobe to something low
    if (current_state == party_strobe_state) {
        set_state(steady_state, RAMP_SIZE/6);
    }
    // in normal mode, step down by half or turn off
    else if (current_state == steady_state) {
        if (actual_level > 1) {
            set_level(actual_level >> 1);
        }
        else {
            set_state(off_state, 0);
        }
    }
}

void setup() {
    set_level(RAMP_SIZE/8);
    delay_4ms(3);
    set_level(0);

    push_state(off_state, 0);
}
