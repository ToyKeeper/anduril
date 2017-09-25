/*
 * Anduril: Narsil-inspired UI for SpaghettiMonster.
 * (Anduril is Aragorn's sword, the blade Narsil reforged)
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
//#define FSM_FW3A_DRIVER
#define USE_LVP
#define USE_THERMAL_REGULATION
#define DEFAULT_THERM_CEIL 50
#define USE_DELAY_MS
#define USE_DELAY_4MS
#define USE_DELAY_ZERO
#define USE_RAMPING
#define USE_SET_LEVEL_GRADUALLY
#define RAMP_LENGTH 150
#define BLINK_AT_RAMP_BOUNDARIES
//#define BLINK_AT_RAMP_FLOOR
#define USE_BATTCHECK
#define BATTCHECK_VpT
#define MAX_CLICKS 5
#define USE_EEPROM
#define EEPROM_BYTES 12
#define USE_IDLE_MODE
//#define HALFSPEED_LEVEL 30  // looks good, but sounds bad
#define HALFSPEED_LEVEL 14
#include "spaghetti-monster.h"

// Options specific to this UI (not inherited from SpaghettiMonster)
#define USE_LIGHTNING_MODE
// set this a bit high, since the bottom 2 ramp levels might not emit any light at all
#define GOODNIGHT_TIME  65  // minutes (approximately)
#define GOODNIGHT_LEVEL 24  // ~11 lm

// FSM states
uint8_t off_state(EventPtr event, uint16_t arg);
// ramping mode and its related config mode
uint8_t steady_state(EventPtr event, uint16_t arg);
uint8_t ramp_config_state(EventPtr event, uint16_t arg);
// party and tactical strobes
uint8_t strobe_state(EventPtr event, uint16_t arg);
#ifdef USE_LIGHTNING_MODE
#define NUM_STROBES 4
#else
#define NUM_STROBES 3
#endif
#ifdef USE_BATTCHECK
uint8_t battcheck_state(EventPtr event, uint16_t arg);
uint8_t tempcheck_state(EventPtr event, uint16_t arg);
uint8_t thermal_config_state(EventPtr event, uint16_t arg);
#endif
// 1-hour ramp down from low, then automatic off
uint8_t goodnight_state(EventPtr event, uint16_t arg);
// beacon mode and its related config mode
uint8_t beacon_state(EventPtr event, uint16_t arg);
uint8_t beacon_config_state(EventPtr event, uint16_t arg);
// soft lockout
#define MOON_DURING_LOCKOUT_MODE
uint8_t lockout_state(EventPtr event, uint16_t arg);
// momentary / signalling mode
uint8_t momentary_state(EventPtr event, uint16_t arg);

// general helper function for config modes
uint8_t number_entry_state(EventPtr event, uint16_t arg);
// return value from number_entry_state()
volatile uint8_t number_entry_value;

void blink_confirm(uint8_t num);

// remember stuff even after battery was changed
void load_config();
void save_config();

// brightness control
uint8_t memorized_level = MAX_1x7135;
// smooth vs discrete ramping
volatile uint8_t ramp_style = 0;  // 0 = smooth, 1 = discrete
volatile uint8_t ramp_smooth_floor = 1;
#if PWM_CHANNELS == 3
volatile uint8_t ramp_smooth_ceil = MAX_Nx7135;
volatile uint8_t ramp_discrete_ceil = MAX_Nx7135;
#else
volatile uint8_t ramp_smooth_ceil = MAX_LEVEL - 30;
volatile uint8_t ramp_discrete_ceil = MAX_LEVEL - 30;
#endif
volatile uint8_t ramp_discrete_floor = 20;
volatile uint8_t ramp_discrete_steps = 7;
uint8_t ramp_discrete_step_size;  // don't set this

// calculate the nearest ramp level which would be valid at the moment
// (is a no-op for smooth ramp, but limits discrete ramp to only the
// correct levels for the user's config)
uint8_t nearest_level(int16_t target);

#ifdef USE_THERMAL_REGULATION
// brightness before thermal step-down
uint8_t target_level = 0;
#endif

// strobe timing
volatile uint8_t strobe_delays[] = { 40, 67 };  // party strobe, tactical strobe
volatile uint8_t strobe_type = 3;  // 0 == party strobe, 1 == tactical strobe, 2 == lightning storm, 3 == bike flasher

// bike mode config options
volatile uint8_t bike_flasher_brightness = MAX_1x7135;

#ifdef USE_LIGHTNING_MODE
volatile uint8_t pseudo_rand_seed = 0;
uint8_t pseudo_rand();
#endif

// beacon timing
volatile uint8_t beacon_seconds = 2;


uint8_t off_state(EventPtr event, uint16_t arg) {
    static uint8_t ticks_spent_awake = 0;
    // turn emitter off when entering state
    if (event == EV_enter_state) {
        set_level(0);
        // sleep while off  (lower power use)
        go_to_standby = 1;
        ticks_spent_awake = 0;
        return MISCHIEF_MANAGED;
    }
    // go back to sleep eventually if we got bumped but didn't leave "off" state
    // FIXME: can I just use arg instead of ticks_spent_awake?
    else if (event == EV_tick) {
        ticks_spent_awake ++;
        if (ticks_spent_awake > 240) {
            ticks_spent_awake = 0;
            go_to_standby = 1;
        }
        return MISCHIEF_MANAGED;
    }
    // hold (initially): go to lowest level, but allow abort for regular click
    else if (event == EV_click1_press) {
        set_level(nearest_level(1));
        return MISCHIEF_MANAGED;
    }
    // 1 click (before timeout): go to memorized level, but allow abort for double click
    else if (event == EV_click1_release) {
        set_level(nearest_level(memorized_level));
        return MISCHIEF_MANAGED;
    }
    // 1 click: regular mode
    else if (event == EV_1click) {
        set_state(steady_state, memorized_level);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks (initial press): off, to prep for later events
    else if (event == EV_click2_press) {
        set_level(0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: highest mode
    else if (event == EV_2clicks) {
        set_state(steady_state, nearest_level(MAX_LEVEL));
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_BATTCHECK
    // 3 clicks: battcheck mode / blinky mode group 1
    else if (event == EV_3clicks) {
        set_state(battcheck_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif
    // click, click, long-click: strobe mode
    else if (event == EV_click3_hold) {
        set_state(strobe_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 4 clicks: soft lockout
    else if (event == EV_4clicks) {
        blink_confirm(2);
        set_state(lockout_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 5 clicks: momentary mode
    else if (event == EV_5clicks) {
        blink_confirm(1);
        set_state(momentary_state, 0);
        return MISCHIEF_MANAGED;
    }
    // hold: go to lowest level
    else if (event == EV_click1_hold) {
        // don't start ramping immediately;
        // give the user time to release at moon level
        if (arg >= HOLD_TIMEOUT) {
            set_state(steady_state, 1);
        }
        return MISCHIEF_MANAGED;
    }
    // hold, release quickly: go to lowest level
    else if (event == EV_click1_hold_release) {
        set_state(steady_state, 1);
        return MISCHIEF_MANAGED;
    }
    // click, hold: go to highest level (for ramping down)
    else if (event == EV_click2_hold) {
        set_state(steady_state, MAX_LEVEL);
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}


uint8_t steady_state(EventPtr event, uint16_t arg) {
    uint8_t mode_min = ramp_smooth_floor;
    uint8_t mode_max = ramp_smooth_ceil;
    uint8_t ramp_step_size = 1;
    if (ramp_style) {
        mode_min = ramp_discrete_floor;
        mode_max = ramp_discrete_ceil;
        ramp_step_size = ramp_discrete_step_size;
    }

    // turn LED on when we first enter the mode
    if (event == EV_enter_state) {
        // remember this level, unless it's moon or turbo
        if ((arg > mode_min) && (arg < mode_max))
            memorized_level = arg;
        // use the requested level even if not memorized
        #ifdef USE_THERMAL_REGULATION
        target_level = arg;
        #endif
        set_level(nearest_level(arg));
        return MISCHIEF_MANAGED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: go to/from highest level
    else if (event == EV_2clicks) {
        if (actual_level < MAX_LEVEL) {
            memorized_level = actual_level;  // in case we're on moon
            #ifdef USE_THERMAL_REGULATION
            target_level = MAX_LEVEL;
            #endif
            // true turbo, not the mode-specific ceiling
            set_level(MAX_LEVEL);
        }
        else {
            #ifdef USE_THERMAL_REGULATION
            target_level = memorized_level;
            #endif
            set_level(memorized_level);
        }
        return MISCHIEF_MANAGED;
    }
    // 3 clicks: toggle smooth vs discrete ramping
    else if (event == EV_3clicks) {
        ramp_style ^= 1;
        memorized_level = nearest_level(memorized_level);
        save_config();
        set_level(0);
        delay_4ms(20/4);
        set_level(memorized_level);
        return MISCHIEF_MANAGED;
    }
    // 4 clicks: configure this ramp mode
    else if (event == EV_4clicks) {
        set_state(ramp_config_state, 0);
        return MISCHIEF_MANAGED;
    }
    // hold: change brightness (brighter)
    else if (event == EV_click1_hold) {
        // ramp slower in discrete mode
        if (ramp_style  &&  (arg % HOLD_TIMEOUT != 0)) {
            return MISCHIEF_MANAGED;
        }
        // TODO? make it ramp down instead, if already at max?
        memorized_level = nearest_level((int16_t)actual_level + ramp_step_size);
        #ifdef USE_THERMAL_REGULATION
        target_level = memorized_level;
        #endif
        #ifdef BLINK_AT_RAMP_BOUNDARIES
        // only blink once for each threshold
        if ((memorized_level != actual_level)
                && ((memorized_level == MAX_1x7135)
                    #if PWM_CHANNELS >= 3
                    || (memorized_level == MAX_Nx7135)
                    #endif
                    || (memorized_level == mode_max))) {
            set_level(0);
            delay_4ms(8/4);
        }
        #endif
        set_level(memorized_level);
        return MISCHIEF_MANAGED;
    }
    // click, hold: change brightness (dimmer)
    else if (event == EV_click2_hold) {
        // ramp slower in discrete mode
        if (ramp_style  &&  (arg % HOLD_TIMEOUT != 0)) {
            return MISCHIEF_MANAGED;
        }
        // TODO? make it ramp up instead, if already at min?
        memorized_level = nearest_level((int16_t)actual_level - ramp_step_size);
        #ifdef USE_THERMAL_REGULATION
        target_level = memorized_level;
        #endif
        #ifdef BLINK_AT_RAMP_BOUNDARIES
        // only blink once for each threshold
        if ((memorized_level != actual_level)
                && ((memorized_level == MAX_1x7135)
                    #if PWM_CHANNELS >= 3
                    || (memorized_level == MAX_Nx7135)
                    #endif
                    #ifdef BLINK_AT_RAMP_FLOOR
                    || (memorized_level == mode_min)
                    #endif
                    )) {
            set_level(0);
            delay_4ms(8/4);
        }
        #endif
        set_level(memorized_level);
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_SET_LEVEL_GRADUALLY
    else if (event == EV_tick) {
        if (!(arg & 7)) gradual_tick();
        //if (!(arg & 3)) gradual_tick();
        //gradual_tick();
        return MISCHIEF_MANAGED;
    }
    #endif
    #ifdef USE_THERMAL_REGULATION
    // overheating: drop by an amount proportional to how far we are above the ceiling
    else if (event == EV_temperature_high) {
        /*
        uint8_t foo = actual_level;
        set_level(0);
        delay_4ms(2);
        set_level(foo);
        */
        if (actual_level > MAX_LEVEL/3) {
            int16_t stepdown = actual_level - arg;
            if (stepdown < MAX_LEVEL/3) stepdown = MAX_LEVEL/3;
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
        /*
        uint8_t foo = actual_level;
        set_level(0);
        delay_4ms(2);
        set_level(foo);
        */
        if (actual_level < target_level) {
            //int16_t stepup = actual_level + (arg>>1);
            int16_t stepup = actual_level + arg;
            if (stepup > target_level) stepup = target_level;
            else if (stepup < MAX_LEVEL/3) stepup = MAX_LEVEL/3;
            #ifdef USE_SET_LEVEL_GRADUALLY
            set_level_gradually(stepup);
            #else
            set_level(stepup);
            #endif
        }
        return MISCHIEF_MANAGED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}


uint8_t strobe_state(EventPtr event, uint16_t arg) {
    if (event == EV_enter_state) {
        return MISCHIEF_MANAGED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: rotate through strobe/flasher modes
    else if (event == EV_2clicks) {
        strobe_type = (strobe_type + 1) % NUM_STROBES;
        interrupt_nice_delays();
        save_config();
        return MISCHIEF_MANAGED;
    }
    // hold: change speed (go faster)
    //       or change brightness (brighter)
    else if (event == EV_click1_hold) {
        if (strobe_type < 2) {
            if ((arg & 1) == 0) {
                if (strobe_delays[strobe_type] > 8) strobe_delays[strobe_type] --;
            }
        }
        // biking mode brighter
        else if (strobe_type == 3) {
            if (bike_flasher_brightness < MAX_LEVEL/2)
                bike_flasher_brightness ++;
            set_level(bike_flasher_brightness);
        }
        return MISCHIEF_MANAGED;
    }
    // click, hold: change speed (go slower)
    //       or change brightness (dimmer)
    else if (event == EV_click2_hold) {
        if (strobe_type < 2) {
            if ((arg & 1) == 0) {
                if (strobe_delays[strobe_type] < 255) strobe_delays[strobe_type] ++;
            }
        }
        // biking mode dimmer
        else if (strobe_type == 3) {
            if (bike_flasher_brightness > 1)
                bike_flasher_brightness --;
            set_level(bike_flasher_brightness);
        }
        return MISCHIEF_MANAGED;
    }
    // release hold: save new strobe settings
    else if ((event == EV_click1_hold_release)
          || (event == EV_click2_hold_release)) {
        save_config();
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_LIGHTNING_MODE
    // clock tick: bump the random seed
    else if (event == EV_tick) {
        pseudo_rand_seed += arg;
        return MISCHIEF_MANAGED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}


#ifdef USE_BATTCHECK
uint8_t battcheck_state(EventPtr event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: goodnight mode
    else if (event == EV_2clicks) {
        set_state(goodnight_state, 0);
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}

uint8_t tempcheck_state(EventPtr event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: battcheck mode
    else if (event == EV_2clicks) {
        set_state(battcheck_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 3 clicks: thermal config mode
    else if (event == EV_3clicks) {
        set_state(thermal_config_state, 0);
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}
#endif


uint8_t beacon_state(EventPtr event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: tempcheck mode
    else if (event == EV_2clicks) {
        set_state(tempcheck_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 3 clicks: beacon config mode
    else if (event == EV_3clicks) {
        set_state(beacon_config_state, 0);
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}


#define GOODNIGHT_TICKS_PER_STEPDOWN (GOODNIGHT_TIME*TICKS_PER_SECOND*60L/GOODNIGHT_LEVEL)
uint8_t goodnight_state(EventPtr event, uint16_t arg) {
    static uint16_t ticks_since_stepdown = 0;
    // blink on start
    if (event == EV_enter_state) {
        ticks_since_stepdown = 0;
        blink_confirm(2);
        set_level(GOODNIGHT_LEVEL);
        return MISCHIEF_MANAGED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: beacon mode
    else if (event == EV_2clicks) {
        set_state(beacon_state, 0);
        return MISCHIEF_MANAGED;
    }
    // tick: step down (maybe) or off (maybe)
    else if (event == EV_tick) {
        if (++ticks_since_stepdown > GOODNIGHT_TICKS_PER_STEPDOWN) {
            ticks_since_stepdown = 0;
            set_level(actual_level-1);
            if (! actual_level) {
                #if 0  // test blink, to help measure timing
                set_level(MAX_LEVEL>>2);
                delay_4ms(8/2);
                set_level(0);
                #endif
                set_state(off_state, 0);
            }
        }
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}


uint8_t lockout_state(EventPtr event, uint16_t arg) {
    static uint8_t ticks_spent_awake = 0;

    #ifdef MOON_DURING_LOCKOUT_MODE
    // momentary(ish) moon mode during lockout
    // not all presses will be counted;
    // it depends on what is in the master event_sequences table
    uint8_t last = 0;
    for(uint8_t i=0; pgm_read_byte(event + i) && (i<EV_MAX_LEN); i++)
        last = pgm_read_byte(event + i);
    if ((last == A_PRESS) || (last == A_HOLD)) {
        // detect moon level and activate it
        uint8_t lvl = ramp_smooth_floor;
        if (ramp_discrete_floor < lvl) lvl = ramp_discrete_floor;
        set_level(lvl);
        ticks_spent_awake = 0;
    }
    else if ((last == A_RELEASE) || (last == A_RELEASE_TIMEOUT)) {
        set_level(0);
    }
    #endif

    // regular event handling
    // conserve power while locked out
    // (allow staying awake long enough to exit, but otherwise
    //  be persistent about going back to sleep every few seconds
    //  even if the user keeps pressing the button)
    if (event == EV_tick) {
        ticks_spent_awake ++;
        if (ticks_spent_awake > 180) {
            ticks_spent_awake = 0;
            go_to_standby = 1;
        }
        return MISCHIEF_MANAGED;
    }
    // 4 clicks: exit
    else if (event == EV_4clicks) {
        blink_confirm(1);
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }

    return EVENT_NOT_HANDLED;
}


uint8_t momentary_state(EventPtr event, uint16_t arg) {
    if (event == EV_click1_press) {
        set_level(memorized_level);
        empty_event_sequence();  // don't attempt to parse multiple clicks
        return MISCHIEF_MANAGED;
    }

    else if (event == EV_release) {
        set_level(0);
        empty_event_sequence();  // don't attempt to parse multiple clicks
        //go_to_standby = 1;  // sleep while light is off
        return MISCHIEF_MANAGED;
    }

    // Sleep, dammit!  (but wait a few seconds first)
    // (because standby mode uses such little power that it can interfere
    //  with exiting via tailcap loosen+tighten unless you leave power
    //  disconnected for several seconds, so we want to be awake when that
    //  happens to speed up the process)
    else if ((event == EV_tick)  &&  (actual_level == 0)) {
        if (arg > TICKS_PER_SECOND*15) {  // sleep after 15 seconds
            go_to_standby = 1;  // sleep while light is off
        }
        return MISCHIEF_MANAGED;
    }

    return EVENT_NOT_HANDLED;
}


uint8_t ramp_config_state(EventPtr event, uint16_t arg) {
    static uint8_t config_step;
    static uint8_t num_config_steps;
    if (event == EV_enter_state) {
        config_step = 0;
        if (ramp_style) {
            num_config_steps = 3;
        }
        else {
            num_config_steps = 2;
        }
        set_level(0);
        return MISCHIEF_MANAGED;
    }
    // advance forward through config steps
    else if (event == EV_tick) {
        if (config_step < num_config_steps) {
            push_state(number_entry_state, config_step + 1);
        }
        else {
            save_config();
            // TODO: blink out some sort of success pattern
            // return to steady mode
            set_state(steady_state, memorized_level);
        }
        return MISCHIEF_MANAGED;
    }
    // an option was set (return from number_entry_state)
    else if (event == EV_reenter_state) {
        #if 0
        // FIXME? this is a kludge which relies on the vars being consecutive
        // in RAM and in the same order as declared
        // ... and it doesn't work; it seems they're not consecutive  :(
        volatile uint8_t *dest;
        if (ramp_style) dest = (&ramp_discrete_floor) + config_step;
        else dest = (&ramp_smooth_floor) + config_step;
        if (number_entry_value)
            *dest = number_entry_value;
        #else
        switch (config_step) {
            case 0:
                if (number_entry_value) {
                    if (ramp_style) ramp_discrete_floor = number_entry_value;
                    else ramp_smooth_floor = number_entry_value;
                }
                break;
            case 1:
                if (number_entry_value) {
                    if (ramp_style) ramp_discrete_ceil = MAX_LEVEL + 1 - number_entry_value;
                    else ramp_smooth_ceil = MAX_LEVEL + 1 - number_entry_value;
                }
                break;
            case 2:
                if (number_entry_value)
                    ramp_discrete_steps = number_entry_value;
                break;
        }
        #endif
        config_step ++;
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}


uint8_t thermal_config_state(EventPtr event, uint16_t arg) {
    static uint8_t done = 0;
    if (event == EV_enter_state) {
        set_level(0);
        done = 0;
        return MISCHIEF_MANAGED;
    }
    // advance forward through config steps
    else if (event == EV_tick) {
        // ask the user for a number
        if (! done) push_state(number_entry_state, 0);
        // return to original mode
        else set_state(tempcheck_state, 0);
        return MISCHIEF_MANAGED;
    }
    // an option was set (return from number_entry_state)
    else if (event == EV_reenter_state) {
        if (number_entry_value) therm_ceil = 30 + number_entry_value;
        if (therm_ceil > MAX_THERM_CEIL) therm_ceil = MAX_THERM_CEIL;
        save_config();
        done = 1;
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}


uint8_t beacon_config_state(EventPtr event, uint16_t arg) {
    static uint8_t done = 0;
    if (event == EV_enter_state) {
        set_level(0);
        done = 0;
        return MISCHIEF_MANAGED;
    }
    // advance forward through config steps
    else if (event == EV_tick) {
        // ask the user for a number
        if (! done) push_state(number_entry_state, 0);
        // return to original mode
        else set_state(beacon_state, 0);
        return MISCHIEF_MANAGED;
    }
    // an option was set (return from number_entry_state)
    else if (event == EV_reenter_state) {
        if (number_entry_value) beacon_seconds = number_entry_value;
        save_config();
        done = 1;
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}


uint8_t number_entry_state(EventPtr event, uint16_t arg) {
    static uint8_t value;
    static uint8_t blinks_left;
    static uint8_t entry_step;
    static uint16_t wait_ticks;
    if (event == EV_enter_state) {
        value = 0;
        blinks_left = arg;
        entry_step = 0;
        wait_ticks = 0;
        // TODO: blink out the 'arg' to show which option this is
        return MISCHIEF_MANAGED;
    }
    // advance through the process:
    // 0: wait a moment
    // 1: blink out the 'arg' value
    // 2: wait a moment
    // 3: "buzz" while counting clicks
    // 4: save and exit
    else if (event == EV_tick) {
        // wait a moment
        if ((entry_step == 0) || (entry_step == 2)) {
            if (wait_ticks < TICKS_PER_SECOND/2)
                wait_ticks ++;
            else {
                entry_step ++;
                wait_ticks = 0;
            }
        }
        // blink out the option number
        else if (entry_step == 1) {
            if (blinks_left) {
                if ((wait_ticks & 31) == 10) {
                    set_level(RAMP_SIZE/4);
                }
                else if ((wait_ticks & 31) == 20) {
                    set_level(0);
                }
                else if ((wait_ticks & 31) == 31) {
                    blinks_left --;
                }
                wait_ticks ++;
            }
            else {
                entry_step ++;
                wait_ticks = 0;
            }
        }
        else if (entry_step == 3) {  // buzz while waiting for a number to be entered
            wait_ticks ++;
            // buzz for N seconds after last event
            if ((wait_ticks & 3) == 0) {
                set_level(RAMP_SIZE/6);
            }
            else if ((wait_ticks & 3) == 2) {
                set_level(RAMP_SIZE/8);
            }
            // time out after 3 seconds
            if (wait_ticks > TICKS_PER_SECOND*3) {
                //number_entry_value = value;
                set_level(0);
                entry_step ++;
            }
        }
        else if (entry_step == 4) {
            number_entry_value = value;
            pop_state();
        }
        return MISCHIEF_MANAGED;
    }
    // count clicks
    else if (event == EV_click1_release) {
        empty_event_sequence();
        if (entry_step == 3) {  // only count during the "buzz"
            value ++;
            wait_ticks = 0;
            // flash briefly
            set_level(RAMP_SIZE/2);
            delay_4ms(8/2);
            set_level(0);
        }
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}


// find the ramp level closest to the target,
// using only the levels which are allowed in the current state
uint8_t nearest_level(int16_t target) {
    // bounds check
    // using int16_t here saves us a bunch of logic elsewhere,
    // by allowing us to correct for numbers < 0 or > 255 in one central place
    uint8_t mode_min = ramp_smooth_floor;
    uint8_t mode_max = ramp_smooth_ceil;
    if (ramp_style) {
        mode_min = ramp_discrete_floor;
        mode_max = ramp_discrete_ceil;
    }
    if (target < mode_min) return mode_min;
    if (target > mode_max) return mode_max;
    // the rest isn't relevant for smooth ramping
    if (! ramp_style) return target;

    uint8_t ramp_range = ramp_discrete_ceil - ramp_discrete_floor;
    ramp_discrete_step_size = ramp_range / (ramp_discrete_steps-1);
    uint8_t this_level = ramp_discrete_floor;

    for(uint8_t i=0; i<ramp_discrete_steps; i++) {
        this_level = ramp_discrete_floor + (i * (uint16_t)ramp_range / (ramp_discrete_steps-1));
        int8_t diff = target - this_level;
        if (diff < 0) diff = -diff;
        if (diff <= (ramp_discrete_step_size>>1))
            return this_level;
    }
    return this_level;
}


void blink_confirm(uint8_t num) {
    for (; num>0; num--) {
        set_level(MAX_LEVEL/4);
        delay_4ms(10/4);
        set_level(0);
        delay_4ms(100/4);
    }
}


#ifdef USE_LIGHTNING_MODE
uint8_t pseudo_rand() {
    static uint16_t offset = 1024;
    // loop from 1024 to 4095
    offset = ((offset + 1) & 0x0fff) | 0x0400;
    pseudo_rand_seed += 0b01010101;  // 85
    return pgm_read_byte(offset) + pseudo_rand_seed;
}
#endif


void load_config() {
    if (load_eeprom()) {
        ramp_style = eeprom[0];
        ramp_smooth_floor = eeprom[1];
        ramp_smooth_ceil = eeprom[2];
        ramp_discrete_floor = eeprom[3];
        ramp_discrete_ceil = eeprom[4];
        ramp_discrete_steps = eeprom[5];
        strobe_type = eeprom[6];  // TODO: move this to eeprom_wl?
        strobe_delays[0] = eeprom[7];
        strobe_delays[1] = eeprom[8];
        bike_flasher_brightness = eeprom[9];
        beacon_seconds = eeprom[10];
        therm_ceil = eeprom[11];
    }
}

void save_config() {
    eeprom[0] = ramp_style;
    eeprom[1] = ramp_smooth_floor;
    eeprom[2] = ramp_smooth_ceil;
    eeprom[3] = ramp_discrete_floor;
    eeprom[4] = ramp_discrete_ceil;
    eeprom[5] = ramp_discrete_steps;
    eeprom[6] = strobe_type;  // TODO: move this to eeprom_wl?
    eeprom[7] = strobe_delays[0];
    eeprom[8] = strobe_delays[1];
    eeprom[9] = bike_flasher_brightness;
    eeprom[10] = beacon_seconds;
    eeprom[11] = therm_ceil;

    save_eeprom();
}


void low_voltage() {
    // "step down" from strobe to something low
    if (current_state == strobe_state) {
        set_state(steady_state, RAMP_SIZE/6);
    }
    // in normal mode, step down by half or turn off
    else if (current_state == steady_state) {
        if (actual_level > 1) {
            set_level((actual_level >> 1) + (actual_level >> 2));
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
    // blink at power-on to let user know power is connected
    set_level(RAMP_SIZE/8);
    delay_4ms(3);
    set_level(0);

    load_config();

    push_state(off_state, 0);
}


void loop() {

    StatePtr state = current_state;

    if (0) {}

    #ifdef USE_IDLE_MODE
    else if ((state == steady_state)
            || (state == goodnight_state)) {
        if (actual_level < HALFSPEED_LEVEL) {
            // run at half speed
            CLKPR = 1<<CLKPCE;
            CLKPR = 1;
        } else {
            // run at full speed
            CLKPR = 1<<CLKPCE;
            CLKPR = 0;
        }
        // doze until next clock tick
        idle_mode();
    }
    else {
        // run at full speed
        CLKPR = 1<<CLKPCE;
        CLKPR = 0;
    }
    #endif

    if (state == strobe_state) {
        // party / tactical strobe
        if (strobe_type < 2) {
            set_level(MAX_LEVEL);
            if (strobe_type == 0) {  // party strobe
                if (strobe_delays[strobe_type] < 42) delay_zero();
                else delay_ms(1);
            } else {  //tactical strobe
                nice_delay_ms(strobe_delays[strobe_type] >> 1);
            }
            set_level(0);
            nice_delay_ms(strobe_delays[strobe_type]);
        }
        #ifdef USE_LIGHTNING_MODE
        // lightning storm
        else if (strobe_type == 2) {
            int16_t brightness;
            uint16_t rand_time;

            // turn the emitter on at a random level,
            // for a random amount of time between 1ms and 32ms
            //rand_time = 1 << (pseudo_rand() % 7);
            rand_time = pseudo_rand() & 63;
            brightness = 1 << (pseudo_rand() % 7);  // 1, 2, 4, 8, 16, 32, 64
            brightness += 1 << (pseudo_rand()&0x03);  // 2 to 80 now
            brightness += pseudo_rand() % brightness;  // 2 to 159 now (w/ low bias)
            if (brightness > MAX_LEVEL) brightness = MAX_LEVEL;
            set_level(brightness);
            if (! nice_delay_ms(rand_time)) return;

            // decrease the brightness somewhat more gradually, like lightning
            uint8_t stepdown = brightness >> 3;
            if (stepdown < 1) stepdown = 1;
            while(brightness > 1) {
                if (! nice_delay_ms(rand_time)) return;
                brightness -= stepdown;
                if (brightness < 0) brightness = 0;
                set_level(brightness);
                /*
                if ((brightness < MAX_LEVEL/2) && (! (pseudo_rand() & 15))) {
                    brightness <<= 1;
                    set_level(brightness);
                }
                */
                if (! (pseudo_rand() & 3)) {
                    if (! nice_delay_ms(rand_time)) return;
                    set_level(brightness>>1);
                }
            }

            // turn the emitter off,
            // for a random amount of time between 1ms and 8192ms
            // (with a low bias)
            rand_time = 1<<(pseudo_rand()%13);
            rand_time += pseudo_rand()%rand_time;
            set_level(0);
            nice_delay_ms(rand_time);

        }
        #endif
        // bike flasher
        else if (strobe_type == 3) {
            uint8_t burst = bike_flasher_brightness << 1;
            for(uint8_t i=0; i<4; i++) {
                set_level(burst);
                if (! nice_delay_ms(5)) return;
                set_level(bike_flasher_brightness);
                if (! nice_delay_ms(65)) return;
            }
            if (! nice_delay_ms(720)) return;
        }
    }

    #ifdef USE_BATTCHECK
    else if (state == battcheck_state) {
        battcheck();
    }
    else if (state == tempcheck_state) {
        blink_num(temperature>>2);
        nice_delay_ms(1000);
    }
    // TODO: blink out therm_ceil during thermal_config_state
    #endif

    else if (state == beacon_state) {
        set_level(memorized_level);
        if (! nice_delay_ms(500)) return;
        set_level(0);
        nice_delay_ms(((beacon_seconds) * 1000) - 500);
    }
}
