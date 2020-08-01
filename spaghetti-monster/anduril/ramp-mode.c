/*
 * ramp-mode.c: Ramping functions for Anduril.
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

#ifndef RAMP_MODE_C
#define RAMP_MODE_C

#include "ramp-mode.h"

#ifdef USE_SUNSET_TIMER
#include "sunset-timer.h"
#endif

uint8_t steady_state(Event event, uint16_t arg) {
    #ifdef USE_REVERSING
    static int8_t ramp_direction = 1;
    #endif
    #if (B_TIMING_OFF == B_RELEASE_T)
    // if the user double clicks, we need to abort turning off,
    // and this stores the level to return to
    static uint8_t level_before_off = 0;
    #endif

    // make sure ramp globals are correct...
    // ... but they already are; no need to do it here
    //ramp_update_config();
    //nearest_level(1);  // same effect, takes less space

    uint8_t mode_min = ramp_floor;
    uint8_t mode_max = ramp_ceil;
    uint8_t step_size;
    if (ramp_style) { step_size = ramp_discrete_step_size; }
    else { step_size = 1; }

    #ifdef USE_SUNSET_TIMER
    // handle the shutoff timer first
    static uint8_t timer_orig_level = 0;
    uint8_t sunset_active = sunset_timer;  // save for comparison
    // clock tick
    sunset_timer_state(event, arg);
    // if the timer was just turned on
    if (sunset_timer  &&  (! sunset_active)) {
        timer_orig_level = actual_level;
    }
    // if the timer just expired, shut off
    else if (sunset_active  &&  (! sunset_timer)) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif  // ifdef USE_SUNSET_TIMER

    // turn LED on when we first enter the mode
    if ((event == EV_enter_state) || (event == EV_reenter_state)) {
        #if defined(USE_MOMENTARY_MODE) && defined(USE_STROBE_STATE)
        momentary_mode = 0;  // 0 = ramping, 1 = strobes
        #endif
        // if we just got back from config mode, go back to memorized level
        if (event == EV_reenter_state) {
            arg = memorized_level;
        }
        // remember this level, unless it's moon or turbo
        if ((arg > mode_min) && (arg < mode_max))
            memorized_level = arg;
        // use the requested level even if not memorized
        arg = nearest_level(arg);
        set_level_and_therm_target(arg);
        #ifdef USE_REVERSING
        ramp_direction = 1;
        #endif
        return MISCHIEF_MANAGED;
    }
    #if (B_TIMING_OFF == B_RELEASE_T)
    // 1 click (early): off, if configured for early response
    else if (event == EV_click1_release) {
        level_before_off = actual_level;
        set_level_and_therm_target(0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks (early): abort turning off, if configured for early response
    else if (event == EV_click2_press) {
        set_level_and_therm_target(level_before_off);
        return MISCHIEF_MANAGED;
    }
    #endif  // if (B_TIMING_OFF == B_RELEASE_T)
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: go to/from highest level
    else if (event == EV_2clicks) {
        uint8_t turbo_level;
        #ifdef USE_SIMPLE_UI
        if (simple_ui_active) { turbo_level = mode_max; }
        else
        #endif
            turbo_level = MAX_LEVEL;

        if (actual_level < turbo_level) {
            // true turbo, not the mode-specific ceiling
            set_level_and_therm_target(turbo_level);
        }
        else {
            set_level_and_therm_target(memorized_level);
        }
        #ifdef USE_SUNSET_TIMER
        timer_orig_level = actual_level;
        #endif
        return MISCHIEF_MANAGED;
    }
    // hold: change brightness (brighter)
    else if (event == EV_click1_hold) {
        // ramp slower in discrete mode
        if (ramp_style  &&  (arg % HOLD_TIMEOUT != 0)) {
            return MISCHIEF_MANAGED;
        }
        #ifdef USE_REVERSING
        // fix ramp direction on first frame if necessary
        if (!arg) {
            // make it ramp down instead, if already at max
            if (actual_level >= mode_max) { ramp_direction = -1; }
            // make it ramp up if already at min
            // (off->hold->stepped_min->release causes this state)
            else if (actual_level <= mode_min) { ramp_direction = 1; }
        }
        // if the button is stuck, err on the side of safety and ramp down
        else if ((arg > TICKS_PER_SECOND * 5) && (actual_level >= mode_max)) {
            ramp_direction = -1;
        }
        #ifdef USE_LOCKOUT_MODE
        // if the button is still stuck, lock the light
        else if ((arg > TICKS_PER_SECOND * 10) && (actual_level <= mode_min)) {
            blip();
            set_state(lockout_state, 0);
        }
        #endif
        memorized_level = nearest_level((int16_t)actual_level \
                          + (step_size * ramp_direction));
        #else
        memorized_level = nearest_level((int16_t)actual_level + step_size);
        #endif
        #if defined(BLINK_AT_RAMP_CEIL) || defined(BLINK_AT_RAMP_MIDDLE)
        // only blink once for each threshold
        if ((memorized_level != actual_level) && (
                0  // for easier syntax below
                #ifdef BLINK_AT_RAMP_MIDDLE_1
                || (memorized_level == BLINK_AT_RAMP_MIDDLE_1)
                #endif
                #ifdef BLINK_AT_RAMP_MIDDLE_2
                || (memorized_level == BLINK_AT_RAMP_MIDDLE_2)
                #endif
                #ifdef BLINK_AT_RAMP_CEIL
                || (memorized_level == mode_max)
                #endif
                #if defined(USE_REVERSING) && defined(BLINK_AT_RAMP_FLOOR)
                || (memorized_level == mode_min)
                #endif
                )) {
            blip();
        }
        #endif
        #if defined(BLINK_AT_STEPS)
        uint8_t foo = ramp_style;
        ramp_style = 1;
        uint8_t nearest = nearest_level((int16_t)actual_level);
        ramp_style = foo;
        // only blink once for each threshold
        if ((memorized_level != actual_level) &&
                    (ramp_style == 0) &&
                    (memorized_level == nearest)
                    )
        {
            blip();
        }
        #endif
        set_level_and_therm_target(memorized_level);
        #ifdef USE_SUNSET_TIMER
        timer_orig_level = actual_level;
        #endif
        return MISCHIEF_MANAGED;
    }
    #if defined(USE_REVERSING) || defined(START_AT_MEMORIZED_LEVEL)
    // reverse ramp direction on hold release
    else if (event == EV_click1_hold_release) {
        #ifdef USE_REVERSING
        ramp_direction = -ramp_direction;
        #endif
        #ifdef START_AT_MEMORIZED_LEVEL
        save_config_wl();
        #endif
        return MISCHIEF_MANAGED;
    }
    #endif
    // click, hold: change brightness (dimmer)
    else if (event == EV_click2_hold) {
        #ifdef USE_REVERSING
        ramp_direction = 1;
        #endif
        // ramp slower in discrete mode
        if (ramp_style  &&  (arg % HOLD_TIMEOUT != 0)) {
            return MISCHIEF_MANAGED;
        }
        // TODO? make it ramp up instead, if already at min?
        memorized_level = nearest_level((int16_t)actual_level - step_size);
        #if defined(BLINK_AT_RAMP_FLOOR) || defined(BLINK_AT_RAMP_MIDDLE)
        // only blink once for each threshold
        if ((memorized_level != actual_level) && (
                0  // for easier syntax below
                #ifdef BLINK_AT_RAMP_MIDDLE_1
                || (memorized_level == BLINK_AT_RAMP_MIDDLE_1)
                #endif
                #ifdef BLINK_AT_RAMP_MIDDLE_2
                || (memorized_level == BLINK_AT_RAMP_MIDDLE_2)
                #endif
                #ifdef BLINK_AT_RAMP_FLOOR
                || (memorized_level == mode_min)
                #endif
                )) {
            blip();
        }
        #endif
        #if defined(BLINK_AT_STEPS)
        uint8_t foo = ramp_style;
        ramp_style = 1;
        uint8_t nearest = nearest_level((int16_t)actual_level);
        ramp_style = foo;
        // only blink once for each threshold
        if ((memorized_level != actual_level) &&
                    (ramp_style == 0) &&
                    (memorized_level == nearest)
                    )
        {
            blip();
        }
        #endif
        set_level_and_therm_target(memorized_level);
        #ifdef USE_SUNSET_TIMER
        timer_orig_level = actual_level;
        #endif
        return MISCHIEF_MANAGED;
    }
    #ifdef START_AT_MEMORIZED_LEVEL
    // click, release, hold, release: save new ramp level (if necessary)
    else if (event == EV_click2_hold_release) {
        save_config_wl();
        return MISCHIEF_MANAGED;
    }
    #endif

    else if (event == EV_tick) {
        #ifdef USE_SUNSET_TIMER
        // reduce output if shutoff timer is active
        if (sunset_timer) {
            uint8_t dimmed_level = timer_orig_level * (sunset_timer-1) / sunset_timer_peak;
            if (dimmed_level < 1) dimmed_level = 1;
            #ifdef USE_SET_LEVEL_GRADUALLY
            set_level_gradually(dimmed_level);
            target_level = dimmed_level;
            #else
            set_level_and_therm_target(dimmed_level);
            #endif
        }
        #endif
        #ifdef USE_REVERSING
        // un-reverse after 1 second
        if (arg == TICKS_PER_SECOND) ramp_direction = 1;
        #endif
        #ifdef USE_SET_LEVEL_GRADUALLY
        int16_t diff = gradual_target - actual_level;
        static uint16_t ticks_since_adjust = 0;
        ticks_since_adjust++;
        if (diff) {
            uint16_t ticks_per_adjust = 256;
            if (diff < 0) {
                //diff = -diff;
                if (actual_level > THERM_FASTER_LEVEL) {
                    #ifdef THERM_HARD_TURBO_DROP
                    ticks_per_adjust >>= 2;
                    #endif
                    ticks_per_adjust >>= 2;
                }
            } else {
                // rise at half speed
                ticks_per_adjust <<= 1;
            }
            while (diff) {
                ticks_per_adjust >>= 1;
                //diff >>= 1;
                diff /= 2;  // because shifting produces weird behavior
            }
            if (ticks_since_adjust > ticks_per_adjust)
            {
                gradual_tick();
                ticks_since_adjust = 0;
            }
        }
        #endif  // ifdef USE_SET_LEVEL_GRADUALLY
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_THERMAL_REGULATION
    // overheating: drop by an amount proportional to how far we are above the ceiling
    else if (event == EV_temperature_high) {
        #if 0
        blip();
        #endif
        #ifdef THERM_HARD_TURBO_DROP
        //if (actual_level > THERM_FASTER_LEVEL) {
        if (actual_level == MAX_LEVEL) {
            #ifdef USE_SET_LEVEL_GRADUALLY
            set_level_gradually(THERM_FASTER_LEVEL);
            target_level = THERM_FASTER_LEVEL;
            #else
            set_level_and_therm_target(THERM_FASTER_LEVEL);
            #endif
        } else
        #endif
        if (actual_level > MIN_THERM_STEPDOWN) {
            int16_t stepdown = actual_level - arg;
            if (stepdown < MIN_THERM_STEPDOWN) stepdown = MIN_THERM_STEPDOWN;
            else if (stepdown > MAX_LEVEL) stepdown = MAX_LEVEL;
            #ifdef USE_SET_LEVEL_GRADUALLY
            set_level_gradually(stepdown);
            #else
            set_level(stepdown);
            #endif
        }
        return MISCHIEF_MANAGED;
    }
    // underheating: increase slowly if we're lower than the target
    //               (proportional to how low we are)
    else if (event == EV_temperature_low) {
        #if 0
        blip();
        #endif
        if (actual_level < target_level) {
            //int16_t stepup = actual_level + (arg>>1);
            int16_t stepup = actual_level + arg;
            if (stepup > target_level) stepup = target_level;
            else if (stepup < MIN_THERM_STEPDOWN) stepup = MIN_THERM_STEPDOWN;
            #ifdef USE_SET_LEVEL_GRADUALLY
            set_level_gradually(stepup);
            #else
            set_level(stepup);
            #endif
        }
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_SET_LEVEL_GRADUALLY
    // temperature is within target window
    // (so stop trying to adjust output)
    else if (event == EV_temperature_okay) {
        // if we're still adjusting output...  stop after the current step
        if (gradual_target > actual_level)
            gradual_target = actual_level + 1;
        else if (gradual_target < actual_level)
            gradual_target = actual_level - 1;
        return MISCHIEF_MANAGED;
    }
    #endif  // ifdef USE_SET_LEVEL_GRADUALLY
    #endif  // ifdef USE_THERMAL_REGULATION

    ////////// Every action below here is blocked in the simple UI //////////
    #ifdef USE_SIMPLE_UI
    if (simple_ui_active) {
        return EVENT_NOT_HANDLED;
    }
    #endif

    // 3 clicks: toggle smooth vs discrete ramping
    else if (event == EV_3clicks) {
        ramp_style = !ramp_style;
        save_config();
        #ifdef START_AT_MEMORIZED_LEVEL
        save_config_wl();
        #endif
        blip();
        memorized_level = nearest_level(actual_level);
        set_level_and_therm_target(memorized_level);
        #ifdef USE_SUNSET_TIMER
        timer_orig_level = actual_level;
        #endif
        return MISCHIEF_MANAGED;
    }

    #ifdef USE_MANUAL_MEMORY
    else if (event == EV_5clicks) {
        manual_memory = actual_level;
        save_config();
        blip();
        return MISCHIEF_MANAGED;
    }
    else if (event == EV_click5_hold) {
        if (0 == arg) {
            manual_memory = 0;
            save_config();
            blip();
        }
        return MISCHIEF_MANAGED;
    }
    #endif

    #ifdef USE_RAMP_CONFIG
    // 7 clicks: configure this ramp mode
    else if (event == EV_7clicks) {
        push_state(ramp_config_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}


#ifdef USE_RAMP_CONFIG
void ramp_config_save() {
    // parse values
    uint8_t val;
    // 0 = smooth ramp, 1 = stepped ramp, 2 = simple UI's ramp
    uint8_t style = ramp_style;
    if (current_state == simple_ui_config_state)  style = 2;

    val = config_state_values[0];
    if (val) { ramp_floors[style] = val; }

    val = config_state_values[1];
    if (val) { ramp_ceils[style] = MAX_LEVEL + 1 - val; }

    if (style) {  // smooth ramp has no third value
        val = config_state_values[2];
        if (val) ramp_stepss[style] = val;
    }
}

uint8_t ramp_config_state(Event event, uint16_t arg) {
    uint8_t num_config_steps = ramp_style + 2;
    return config_state_base(event, arg,
                             num_config_steps, ramp_config_save);
}

#ifdef USE_SIMPLE_UI
uint8_t simple_ui_config_state(Event event, uint16_t arg) {
    return config_state_base(event, arg, 3, ramp_config_save);
}
#endif
#endif  // #ifdef USE_RAMP_CONFIG

// find the ramp level closest to the target,
// using only the levels which are allowed in the current state
uint8_t nearest_level(int16_t target) {
    ramp_update_config();

    // bounds check
    // using int16_t here saves us a bunch of logic elsewhere,
    // by allowing us to correct for numbers < 0 or > 255 in one central place
    uint8_t mode_min = ramp_floor;
    uint8_t mode_max = ramp_ceil;
    if (target < mode_min) return mode_min;
    if (target > mode_max) return mode_max;
    // the rest isn't relevant for smooth ramping
    if (! ramp_style) return target;

    uint8_t ramp_range = mode_max - mode_min;
    uint8_t num_steps = ramp_stepss[1 + simple_ui_active];
    ramp_discrete_step_size = ramp_range / (num_steps-1);
    uint8_t this_level = mode_min;

    for(uint8_t i=0; i<num_steps; i++) {
        this_level = mode_min + (i * (uint16_t)ramp_range / (num_steps-1));
        int16_t diff = target - this_level;
        if (diff < 0) diff = -diff;
        if (diff <= (ramp_discrete_step_size>>1))
            return this_level;
    }
    return this_level;
}

// ensure ramp globals are correct
void ramp_update_config() {
    uint8_t which = ramp_style;
    if (simple_ui_active) { which = 2; }

    ramp_floor = ramp_floors[which];
    ramp_ceil = ramp_ceils[which];
}

#ifdef USE_THERMAL_REGULATION
void set_level_and_therm_target(uint8_t level) {
    target_level = level;
    set_level(level);
}
#else
#define set_level_and_therm_target(level) set_level(level)
#endif


#endif

