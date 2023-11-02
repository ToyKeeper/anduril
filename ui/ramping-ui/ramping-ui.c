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

#include "hwdef-Emisar_D4.h"
#define USE_LVP
#define USE_THERMAL_REGULATION
#define DEFAULT_THERM_CEIL 32
#define USE_DELAY_MS
#define USE_DELAY_ZERO
#define USE_RAMPING
#define USE_BATTCHECK
#define BATTCHECK_VpT
#define RAMP_LENGTH 150
#include "spaghetti-monster.h"

// FSM states
uint8_t off_state(Event event, uint16_t arg);
uint8_t steady_state(Event event, uint16_t arg);
uint8_t strobe_state(Event event, uint16_t arg);
#ifdef USE_BATTCHECK
uint8_t battcheck_state(Event event, uint16_t arg);
uint8_t tempcheck_state(Event event, uint16_t arg);
#endif

// brightness control
uint8_t memorized_level = MAX_1x7135;
// smooth vs discrete ramping
uint8_t ramp_step_size = 1;

#ifdef USE_THERMAL_REGULATION
// brightness before thermal step-down
uint8_t target_level = 0;
#endif

// strobe timing
volatile uint8_t strobe_delay = 67;
volatile uint8_t strobe_type = 0;  // 0 == party strobe, 1 == tactical strobe


uint8_t off_state(Event event, uint16_t arg) {
    // turn emitter off when entering state
    if (event == EV_enter_state) {
        set_level(0);
        // sleep while off  (lower power use)
        go_to_standby = 1;
        return EVENT_HANDLED;
    }
    // hold (initially): go to lowest level, but allow abort for regular click
    else if (event == EV_click1_press) {
        set_level(1);
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
    // 2 clicks (initial press): off, to prep for later events
    else if (event == EV_click2_press) {
        set_level(0);
        return EVENT_HANDLED;
    }
    // 2 clicks: highest mode
    else if (event == EV_2clicks) {
        set_state(steady_state, MAX_LEVEL);
        return EVENT_HANDLED;
    }
    // 3 clicks: strobe mode
    else if (event == EV_3clicks) {
        set_state(strobe_state, 0);
        return EVENT_HANDLED;
    }
    #ifdef USE_BATTCHECK
    // 4 clicks: battcheck mode
    else if (event == EV_4clicks) {
        set_state(battcheck_state, 0);
        return EVENT_HANDLED;
    }
    #endif
    // hold: go to lowest level
    else if (event == EV_click1_hold) {
        // don't start ramping immediately;
        // give the user time to release at moon level
        if (arg >= HOLD_TIMEOUT)
            set_state(steady_state, 1);
        return EVENT_HANDLED;
    }
    // hold, release quickly: go to lowest level
    else if (event == EV_click1_hold_release) {
        set_state(steady_state, 1);
        return EVENT_HANDLED;
    }
    // click, hold: go to highest level (for ramping down)
    else if (event == EV_click2_hold) {
        set_state(steady_state, MAX_LEVEL);
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}


uint8_t steady_state(Event event, uint16_t arg) {
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
        return EVENT_HANDLED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
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
        return EVENT_HANDLED;
    }
    // 3 clicks: go to strobe modes
    else if (event == EV_3clicks) {
        set_state(strobe_state, 0);
        return EVENT_HANDLED;
    }
    // 4 clicks: toggle smooth vs discrete ramping
    else if (event == EV_4clicks) {
        if (ramp_step_size == 1) ramp_step_size = MAX_LEVEL/6;
        else ramp_step_size = 1;
        set_level(0);
        delay_4ms(20/4);
        set_level(memorized_level);
        return EVENT_HANDLED;
    }
    // hold: change brightness (brighter)
    else if (event == EV_click1_hold) {
        // ramp slower in discrete mode
        if (arg % ramp_step_size != 0) {
            return EVENT_HANDLED;
        }
        // FIXME: make it ramp down instead, if already at max
        if (actual_level + ramp_step_size < MAX_LEVEL)
            memorized_level = actual_level + ramp_step_size;
        else memorized_level = MAX_LEVEL;
        #ifdef USE_THERMAL_REGULATION
        target_level = memorized_level;
        #endif
        // only blink once for each threshold
        if ((memorized_level != actual_level)
                && ((memorized_level == MAX_1x7135)
                    || (memorized_level == MAX_LEVEL))) {
            set_level(0);
            delay_4ms(8/4);
        }
        set_level(memorized_level);
        return EVENT_HANDLED;
    }
    // click, hold: change brightness (dimmer)
    else if (event == EV_click2_hold) {
        // ramp slower in discrete mode
        if (arg % ramp_step_size != 0) {
            return EVENT_HANDLED;
        }
        // FIXME: make it ramp up instead, if already at min
        if (actual_level > ramp_step_size)
            memorized_level = (actual_level-ramp_step_size);
        else
            memorized_level = 1;
        #ifdef USE_THERMAL_REGULATION
        target_level = memorized_level;
        #endif
        // only blink once for each threshold
        if ((memorized_level != actual_level)
                && ((memorized_level == MAX_1x7135)
                    || (memorized_level == 1))) {
            set_level(0);
            delay_4ms(8/4);
        }
        set_level(memorized_level);
        return EVENT_HANDLED;
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
        return EVENT_HANDLED;
    }
    // underheating: increase slowly if we're lower than the target
    //               (proportional to how low we are)
    else if (event == EV_temperature_low) {
        if (actual_level < target_level) {
            uint8_t stepup = actual_level + (arg>>1);
            if (stepup > target_level) stepup = target_level;
            set_level(stepup);
        }
        return EVENT_HANDLED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}


uint8_t strobe_state(Event event, uint16_t arg) {
    if (event == EV_enter_state) {
        return EVENT_HANDLED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    // 2 clicks: toggle party strobe vs tactical strobe
    else if (event == EV_2clicks) {
        strobe_type ^= 1;
        return EVENT_HANDLED;
    }
    // 3 clicks: go back to regular modes
    else if (event == EV_3clicks) {
        set_state(steady_state, memorized_level);
        return EVENT_HANDLED;
    }
    // hold: change speed (go faster)
    else if (event == EV_click1_hold) {
        if ((arg & 1) == 0) {
            if (strobe_delay > 8) strobe_delay --;
        }
        return EVENT_HANDLED;
    }
    // click, hold: change speed (go slower)
    else if (event == EV_click2_hold) {
        if ((arg & 1) == 0) {
            if (strobe_delay < 255) strobe_delay ++;
        }
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}


#ifdef USE_BATTCHECK
uint8_t battcheck_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    // 2 clicks: tempcheck mode
    else if (event == EV_2clicks) {
        set_state(tempcheck_state, 0);
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}

uint8_t tempcheck_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}
#endif


void low_voltage() {
    // "step down" from strobe to something low
    if (current_state == strobe_state) {
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
    // all other modes, just turn off when voltage is low
    else {
        set_state(off_state, 0);
    }
}


void setup() {
    set_level(RAMP_SIZE/8);
    delay_4ms(3);
    set_level(0);

    push_state(off_state, 0);
}


void loop() {
    if (current_state == strobe_state) {
        set_level(MAX_LEVEL);
        if (strobe_type == 0) {  // party strobe
            if (strobe_delay < 30) delay_zero();
            else delay_ms(1);
        } else {  //tactical strobe
            nice_delay_ms(strobe_delay >> 1);
        }
        set_level(0);
        nice_delay_ms(strobe_delay);
    }
    #ifdef USE_BATTCHECK
    else if (current_state == battcheck_state) {
        battcheck();
    }
    else if (current_state == tempcheck_state) {
        blink_num(temperature);
        nice_delay_ms(1000);
    }
    #endif
}
