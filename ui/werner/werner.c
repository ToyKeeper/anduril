/*
 * Werner: Werner-style dual-switch UI for SpaghettiMonster.
 * Side click to go up, side hold to go down, tail click for on/off.
 *
 * Copyright (C) 2018 Selene ToyKeeper
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

/********* User-configurable options *********/
// Physical driver type (uncomment one of the following or define it at the gcc command line)
//#define CONFIGFILE cfg-emisar-d4.h

#define USE_LVP  // FIXME: won't build when this option is turned off

// parameters for this defined below or per-driver
#define USE_THERMAL_REGULATION
#define DEFAULT_THERM_CEIL 45  // try not to get hotter than this

// battery readout style (pick one)
#define BATTCHECK_VpT
//#define BATTCHECK_8bars  // FIXME: breaks build
//#define BATTCHECK_4bars  // FIXME: breaks build

// cut clock speed at very low modes for better efficiency
// (defined here so config files can override it)
#define USE_DYNAMIC_UNDERCLOCKING

/***** specific settings for known driver types *****/
#ifdef CONFIGFILE
#include "fsm/tk.h"
#include incfile(CONFIGFILE)
#else
#error You need to define CONFIGFILE
#endif

// thermal properties, if not defined per-driver
#ifndef MIN_THERM_STEPDOWN
#define MIN_THERM_STEPDOWN MAX_1x7135  // lowest value it'll step down to
#endif
#ifndef THERM_FASTER_LEVEL
    #ifdef MAX_Nx7135
    #define THERM_FASTER_LEVEL MAX_Nx7135  // throttle back faster when high
    #else
    #define THERM_FASTER_LEVEL (RAMP_SIZE*4/5)  // throttle back faster when high
    #endif
#endif
#ifdef USE_THERMAL_REGULATION
#define USE_SET_LEVEL_GRADUALLY  // isn't used except for thermal adjustments
#endif


/********* Configure SpaghettiMonster *********/
#define USE_DELAY_ZERO
#define USE_RAMPING
#define RAMP_LENGTH 150  // default, if not overridden in a driver cfg file
#define USE_BATTCHECK
#define USE_IDLE_MODE  // reduce power use while awake and no tasks are pending

// auto-detect how many eeprom bytes
#define USE_EEPROM
#ifdef USE_THERMAL_REGULATION
#define EEPROM_BYTES 5
#else
#define EEPROM_BYTES 3
#endif
// for mode memory on tail switch
#define USE_EEPROM_WL
#define EEPROM_WL_BYTES 1

#include "spaghetti-monster.h"


// FSM states
uint8_t off_state(Event event, uint16_t arg);
// simple numeric entry config menu
uint8_t config_state_base(Event event, uint16_t arg,
                          uint8_t num_config_steps,
                          void (*savefunc)());
#define MAX_CONFIG_VALUES 3
uint8_t config_state_values[MAX_CONFIG_VALUES];
// ramping mode and its related config mode
uint8_t steady_state(Event event, uint16_t arg);
uint8_t ramp_config_state(Event event, uint16_t arg);
#ifdef USE_BATTCHECK
uint8_t battcheck_state(Event event, uint16_t arg);
#endif
#ifdef USE_THERMAL_REGULATION
uint8_t tempcheck_state(Event event, uint16_t arg);
uint8_t thermal_config_state(Event event, uint16_t arg);
#endif

// general helper function for config modes
uint8_t number_entry_state(Event event, uint16_t arg);
// return value from number_entry_state()
volatile uint8_t number_entry_value;

void blink_confirm(uint8_t num);

// remember stuff even after battery was changed
void load_config();
void save_config();
void save_config_wl();

// default ramp options if not overridden earlier per-driver
#ifndef RAMP_DISCRETE_FLOOR
  #define RAMP_DISCRETE_FLOOR 1
#endif
#ifndef RAMP_DISCRETE_CEIL
  #define RAMP_DISCRETE_CEIL RAMP_SIZE
#endif
#ifndef RAMP_DISCRETE_STEPS
  #define RAMP_DISCRETE_STEPS 7
#endif

// brightness control
uint8_t memorized_level = MAX_1x7135;
// smooth vs discrete ramping
volatile uint8_t ramp_discrete_floor = RAMP_DISCRETE_FLOOR;
volatile uint8_t ramp_discrete_ceil = RAMP_DISCRETE_CEIL;
volatile uint8_t ramp_discrete_steps = RAMP_DISCRETE_STEPS;
uint8_t ramp_discrete_step_size;  // don't set this

// calculate the nearest ramp level which would be valid at the moment
// (is a no-op for smooth ramp, but limits discrete ramp to only the
// correct levels for the user's config)
uint8_t nearest_level(int16_t target);

#ifdef USE_THERMAL_REGULATION
// brightness before thermal step-down
uint8_t target_level = 0;
#endif


uint8_t off_state(Event event, uint16_t arg) {
    // turn emitter off when entering state
    if ((event == EV_enter_state) || (event == EV_reenter_state)) {
        // let the user know the power is connected
        blink_confirm(1);
        // but otherwise stay off
        set_level(0);
        // sleep while off  (lower power use)
        go_to_standby = 1;
        return EVENT_HANDLED;
    }
    // go back to sleep eventually if we got bumped but didn't leave "off" state
    else if (event == EV_tick) {
        if (arg > TICKS_PER_SECOND*2) {
            go_to_standby = 1;
        }
        return EVENT_HANDLED;
    }
    // hold (initially): go to lowest level, but allow abort for regular click
    else if (event == EV_click1_press) {
        set_level(nearest_level(1));
        return EVENT_HANDLED;
    }
    // hold: go to lowest level
    else if (event == EV_click1_hold) {
        // don't start ramping immediately;
        // give the user time to release at moon level
        if (arg >= HOLD_TIMEOUT) {
            set_state(steady_state, 1);
        }
        return EVENT_HANDLED;
    }
    // hold, release quickly: go to lowest level
    else if (event == EV_click1_hold_release) {
        set_state(steady_state, 1);
        return EVENT_HANDLED;
    }
    // 1 click (before timeout): go to memorized level, but allow abort for double click
    else if (event == EV_click1_release) {
        set_level(nearest_level(memorized_level));
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
    // click, hold: go to highest level (for ramping down)
    else if (event == EV_click2_hold) {
        set_state(steady_state, MAX_LEVEL);
        return EVENT_HANDLED;
    }
    // 2 clicks: highest mode
    else if (event == EV_2clicks) {
        set_state(steady_state, nearest_level(MAX_LEVEL));
        return EVENT_HANDLED;
    }
    #ifdef USE_BATTCHECK
    // 3 clicks: battcheck mode / blinky mode group
    else if (event == EV_3clicks) {
        set_state(battcheck_state, 0);
        return EVENT_HANDLED;
    }
    #endif
    // 4 clicks: configure ramp
    else if (event == EV_4clicks) {
        push_state(ramp_config_state, 0);
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}


uint8_t steady_state(Event event, uint16_t arg) {
    uint8_t mode_min = ramp_discrete_floor;
    uint8_t mode_max = ramp_discrete_ceil;
    uint8_t ramp_step_size = ramp_discrete_step_size;

    // turn LED on when we first enter the mode
    if ((event == EV_enter_state) || (event == EV_reenter_state)) {
        // if we just got back from config mode, go back to memorized level
        if (event == EV_reenter_state) {
            arg = memorized_level;
        }
        // remember this level, unless it's moon or turbo
        if ((arg > mode_min) && (arg < mode_max))
            memorized_level = arg;
        // use the requested level even if not memorized
        #ifdef USE_THERMAL_REGULATION
        target_level = arg;
        #endif
        set_level(nearest_level(arg));
        return EVENT_HANDLED;
    }
    // click: brighter
    else if (event == EV_click1_release) {
        memorized_level = nearest_level((int16_t)actual_level + ramp_step_size);
        #ifdef USE_THERMAL_REGULATION
        target_level = memorized_level;
        #endif
        set_level(memorized_level);
        // make sure next click will respond quickly
        empty_event_sequence();
        // remember mode for later
        save_config_wl();
        return EVENT_HANDLED;
    }
    // hold: dimmer
    else if (event == EV_click1_hold) {
        // ramp slower in discrete mode
        if (arg % HOLD_TIMEOUT != 0) {
            return EVENT_HANDLED;
        }
        memorized_level = nearest_level((int16_t)actual_level - ramp_step_size);
        #ifdef USE_THERMAL_REGULATION
        target_level = memorized_level;
        #endif
        set_level(memorized_level);
        return EVENT_HANDLED;
    }
    // reverse ramp direction on hold release
    else if (event == EV_click1_hold_release) {
        save_config_wl();
        return EVENT_HANDLED;
    }
    #if defined(USE_SET_LEVEL_GRADUALLY)
    // gradual thermal regulation
    else if (event == EV_tick) {
        #ifdef USE_SET_LEVEL_GRADUALLY
        // make thermal adjustment speed scale with magnitude
        if ((arg & 1) && (actual_level < THERM_FASTER_LEVEL)) {
            return EVENT_HANDLED;  // adjust slower when not a high mode
        }
        #ifdef THERM_HARD_TURBO_DROP
        else if ((! (actual_level < THERM_FASTER_LEVEL))
                && (actual_level > gradual_target)) {
            gradual_tick();
        }
        else {
        #endif
        // [int(62*4 / (x**0.95)) for x in (1,2,4,8,16,32,64,128)]
        uint8_t intervals[] = {248, 128, 66, 34, 17, 9, 4, 2};
        uint8_t diff;
        static uint8_t ticks_since_adjust = 0;
        ticks_since_adjust ++;
        if (gradual_target > actual_level) diff = gradual_target - actual_level;
        else {
            diff = actual_level - gradual_target;
        }
        uint8_t magnitude = 0;
        #ifndef THERM_HARD_TURBO_DROP
        // if we're on a really high mode, drop faster
        if (actual_level >= THERM_FASTER_LEVEL) { magnitude ++; }
        #endif
        while (diff) {
            magnitude ++;
            diff >>= 1;
        }
        uint8_t ticks_per_adjust = intervals[magnitude];
        if (ticks_since_adjust > ticks_per_adjust)
        {
            gradual_tick();
            ticks_since_adjust = 0;
        }
        //if (!(arg % ticks_per_adjust)) gradual_tick();
        #ifdef THERM_HARD_TURBO_DROP
        }
        #endif
        #endif
        return EVENT_HANDLED;
    }
    #endif
    #ifdef USE_THERMAL_REGULATION
    // overheating: drop by an amount proportional to how far we are above the ceiling
    else if (event == EV_temperature_high) {
        #ifdef THERM_HARD_TURBO_DROP
        if (actual_level > THERM_FASTER_LEVEL) {
            #ifdef USE_SET_LEVEL_GRADUALLY
            set_level_gradually(THERM_FASTER_LEVEL);
            #else
            set_level(THERM_FASTER_LEVEL);
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
        return EVENT_HANDLED;
    }
    // underheating: increase slowly if we're lower than the target
    //               (proportional to how low we are)
    else if (event == EV_temperature_low) {
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
        return EVENT_HANDLED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}


#ifdef USE_BATTCHECK
uint8_t battcheck_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    #ifdef USE_THERMAL_REGULATION
    // 2 clicks: tempcheck mode
    else if (event == EV_2clicks) {
        blink_confirm(2);
        set_state(tempcheck_state, 0);
        return EVENT_HANDLED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}
#endif

#ifdef USE_THERMAL_REGULATION
uint8_t tempcheck_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    // 2 clicks: battcheck mode
    else if (event == EV_2clicks) {
        blink_confirm(1);
        set_state(battcheck_state, 0);
        return EVENT_HANDLED;
    }
    // 4 clicks: thermal config mode
    else if (event == EV_4clicks) {
        push_state(thermal_config_state, 0);
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}
#endif


// ask the user for a sequence of numbers, then save them and return to caller
uint8_t config_state_base(Event event, uint16_t arg,
                          uint8_t num_config_steps,
                          void (*savefunc)()) {
    static uint8_t config_step;
    if (event == EV_enter_state) {
        config_step = 0;
        set_level(0);
        return EVENT_HANDLED;
    }
    // advance forward through config steps
    else if (event == EV_tick) {
        if (config_step < num_config_steps) {
            push_state(number_entry_state, config_step + 1);
        }
        else {
            // TODO: blink out some sort of success pattern
            savefunc();
            save_config();
            //set_state(retstate, retval);
            pop_state();
        }
        return EVENT_HANDLED;
    }
    // an option was set (return from number_entry_state)
    else if (event == EV_reenter_state) {
        config_state_values[config_step] = number_entry_value;
        config_step ++;
        return EVENT_HANDLED;
    }
    //return EVENT_NOT_HANDLED;
    // eat all other events; don't pass any through to parent
    return EVENT_HANDLED;
}

void ramp_config_save() {
    // parse values
    uint8_t val;

    val = config_state_values[0];
    if (val) { ramp_discrete_floor = val; }

    val = config_state_values[1];
    if (val) { ramp_discrete_ceil = MAX_LEVEL + 1 - val; }

    val = config_state_values[2];
    if (val) ramp_discrete_steps = val;
}

uint8_t ramp_config_state(Event event, uint16_t arg) {
    uint8_t num_config_steps;
    num_config_steps = 3;
    return config_state_base(event, arg,
                             num_config_steps, ramp_config_save);
}


#ifdef USE_THERMAL_REGULATION
void thermal_config_save() {
    // parse values
    uint8_t val;

    // calibrate room temperature
    val = config_state_values[0];
    if (val) {
        int8_t rawtemp = temperature - therm_cal_offset;
        therm_cal_offset = val - rawtemp;
        reset_thermal_history = 1;  // invalidate all recent temperature data
    }

    val = config_state_values[1];
    if (val) {
        // set maximum heat limit
        therm_ceil = 30 + val - 1;
    }
    if (therm_ceil > MAX_THERM_CEIL) therm_ceil = MAX_THERM_CEIL;
}

uint8_t thermal_config_state(Event event, uint16_t arg) {
    return config_state_base(event, arg,
                             2, thermal_config_save);
}
#endif


uint8_t number_entry_state(Event event, uint16_t arg) {
    static uint8_t value;
    static uint8_t blinks_left;
    static uint8_t entry_step;
    static uint16_t wait_ticks;
    if (event == EV_enter_state) {
        value = 0;
        blinks_left = arg;
        entry_step = 0;
        wait_ticks = 0;
        return EVENT_HANDLED;
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
        return EVENT_HANDLED;
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
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}


// find the ramp level closest to the target,
// using only the levels which are allowed in the current state
uint8_t nearest_level(int16_t target) {
    // bounds check
    // using int16_t here saves us a bunch of logic elsewhere,
    // by allowing us to correct for numbers < 0 or > 255 in one central place
    uint8_t mode_min = ramp_discrete_floor;
    uint8_t mode_max = ramp_discrete_ceil;
    if (target < mode_min) return mode_min;
    if (target > mode_max) return mode_max;

    uint8_t ramp_range = ramp_discrete_ceil - ramp_discrete_floor;
    ramp_discrete_step_size = ramp_range / (ramp_discrete_steps-1);
    uint8_t this_level = ramp_discrete_floor;

    for(uint8_t i=0; i<ramp_discrete_steps; i++) {
        this_level = ramp_discrete_floor + (i * (uint16_t)ramp_range / (ramp_discrete_steps-1));
        int16_t diff = target - this_level;
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


void load_config() {
    if (load_eeprom()) {
        ramp_discrete_floor = eeprom[0];
        ramp_discrete_ceil = eeprom[1];
        ramp_discrete_steps = eeprom[2];
        #ifdef USE_THERMAL_REGULATION
        therm_ceil = eeprom[3];
        therm_cal_offset = eeprom[4];
        #endif
    }
    if (load_eeprom_wl()) {
        memorized_level = eeprom_wl[0];
    }
}


void save_config() {
    eeprom[0] = ramp_discrete_floor;
    eeprom[1] = ramp_discrete_ceil;
    eeprom[2] = ramp_discrete_steps;
    #ifdef USE_THERMAL_REGULATION
    eeprom[3] = therm_ceil;
    eeprom[4] = therm_cal_offset;
    #endif

    save_eeprom();
}


void save_config_wl() {
    eeprom_wl[0] = memorized_level;
    save_eeprom_wl();
}


void low_voltage() {
    StatePtr state = current_state;

    // in normal mode, step down or turn off
    if (state == steady_state) {
        if (actual_level > 1) {
            uint8_t lvl = (actual_level >> 1) + (actual_level >> 2);
            set_level(lvl);
            #ifdef USE_THERMAL_REGULATION
            target_level = lvl;
            #endif
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
    // dual switch: e-switch + power clicky
    // power clicky acts as a momentary mode
    load_config();

    if (button_is_pressed())
        // hold button to go to moon
        push_state(off_state, 0);
    else
        // otherwise use memory
        push_state(steady_state, memorized_level);
}


void loop() {

    StatePtr state = current_state;

    if (0) {}

    #ifdef USE_BATTCHECK
    else if (state == battcheck_state) {
        battcheck();
    }
    #endif
    #ifdef USE_THERMAL_REGULATION
    // TODO: blink out therm_ceil during thermal_config_state
    else if (state == tempcheck_state) {
        blink_num(temperature);
        nice_delay_ms(1000);
    }
    #endif

    #ifdef USE_IDLE_MODE
    else {
        // doze until next clock tick
        idle_mode();
    }
    #endif

}
