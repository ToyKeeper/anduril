/*
 * Anduril: Narsil-inspired UI for SpaghettiMonster.
 * (Anduril is Aragorn's sword, the blade Narsil reforged)
 *
 * Copyright (C) 2017-2019 Selene ToyKeeper
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

// Usually a program would be structured like this...
// - Library headers
// - App headers
// - App code
//
// ... in each source file.
// ... and each library and part of the program would be linked together.
//
// But this doesn't follow that pattern, because it's using the
//   -fwhole-program
// flag to reduce the compiled size.  It lets us fit more features
// in a tiny MCU chip's ROM.
//
// So the structure is like this instead...
// - App-level configuration headers
//   - Default config
//   - Per build target config
// - Library-level configuration headers
// - Library code (FSM itself)
// - App headers
// - App code (all of it)
//
// Don't do this in regular programs.  It's weird and kind of gross.
// But in this case it gives us a bunch of much-needed space, so...

/********* User-configurable options *********/
#include "cfg-default.h"

/***** specific settings for known driver types *****/
// Anduril config file name (set it here or define it at the gcc command line)
//#define CONFIGFILE cfg-blf-q8.h

#include "tk.h"
#include incfile(CONFIGFILE)


////////// Include headers which need to be before FSM //////////
#ifdef USE_FACTORY_RESET
#include "factory-reset-fsm.h"
#endif

// enable FSM features needed by strobe modes
#include "strobes-fsm.h"


// brightness to use when no memory is set
#ifndef DEFAULT_LEVEL
#define DEFAULT_LEVEL MAX_1x7135
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
#ifndef RAMP_LENGTH
#define RAMP_LENGTH 150  // default, if not overridden in a driver cfg file
#endif
#define MAX_BIKING_LEVEL 120  // should be 127 or less
#define USE_BATTCHECK

#if defined(USE_SIMPLE_UI)
// start in the simple UI after each factory reset?
#ifndef DEFAULT_SIMPLE_UI_ACTIVE
#define DEFAULT_SIMPLE_UI_ACTIVE 1
#endif
#ifndef DEFAULT_SIMPLE_UI_FLOOR
#define DEFAULT_SIMPLE_UI_FLOOR 22
#endif
#ifndef DEFAULT_SIMPLE_UI_CEIL
#define DEFAULT_SIMPLE_UI_CEIL (MAX_1x7135+20)
#endif
#ifndef DEFAULT_SIMPLE_UI_STEPS
#define DEFAULT_SIMPLE_UI_STEPS 3
#endif
#endif

#define USE_IDLE_MODE  // reduce power use while awake and no tasks are pending

// auto-detect how many eeprom bytes
#define USE_EEPROM
typedef enum {
    ramp_style_e,
    #ifdef USE_RAMP_CONFIG
    ramp_smooth_floor_e,
    ramp_smooth_ceil_e,
    ramp_discrete_floor_e,
    ramp_discrete_ceil_e,
    ramp_discrete_steps_e,
    #endif
    #ifdef USE_MANUAL_MEMORY
    manual_memory_e,
    #endif
    #ifdef USE_TINT_RAMPING
    tint_e,
    #endif
    #ifdef USE_STROBE_STATE
    strobe_type_e,
    #endif
    #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
    strobe_delays_0_e,
    strobe_delays_1_e,
    #endif
    #ifdef USE_BIKE_FLASHER_MODE
    bike_flasher_brightness_e,
    #endif
    #ifdef USE_BEACON_MODE
    beacon_seconds_e,
    #endif
    #ifdef USE_SIMPLE_UI
    simple_ui_active_e,
    simple_ui_floor_e,
    simple_ui_ceil_e,
    simple_ui_steps_e,
    #endif
    #ifdef USE_THERMAL_REGULATION
    therm_ceil_e,
    therm_cal_offset_e,
    #endif
    #ifdef USE_INDICATOR_LED
    indicator_led_mode_e,
    #endif
    #ifdef USE_AUX_RGB_LEDS
    rgb_led_off_mode_e,
    rgb_led_lockout_mode_e,
    #endif
    eeprom_indexes_e_END
} eeprom_indexes_e;
#define EEPROM_BYTES eeprom_indexes_e_END

#ifdef START_AT_MEMORIZED_LEVEL
#define USE_EEPROM_WL
#define EEPROM_WL_BYTES 1
#endif

#include "spaghetti-monster.h"


////////// Include all the extra headers //////////

#include "config-mode.h"
#include "lockout.h"

#ifdef USE_TINT_RAMPING
#include "tint-ramping.h"
#endif

#ifdef USE_FACTORY_RESET
#include "factory-reset.h"
#endif

#ifdef USE_BEACON_MODE
#include "beacon.h"
#endif

// this one detects its own enable/disable settings
#include "strobes.h"


// configure the timing of turning on/off in regular ramp mode
// press: react as soon as the button is pressed
#define B_PRESS_T 0
// release: react as soon as the button is released
#define B_RELEASE_T 1
// timeout: react as soon as we're sure the user isn't doing a double-click
#define B_TIMEOUT_T 2
// defaults are release on, timeout off
#ifndef B_TIMING_ON
//#define B_TIMING_ON B_PRESS_T
#define B_TIMING_ON B_RELEASE_T
#endif
#ifndef B_TIMING_OFF
//#define B_TIMING_OFF B_RELEASE_T
#define B_TIMING_OFF B_TIMEOUT_T
#endif


// FSM states
uint8_t off_state(Event event, uint16_t arg);
// ramping mode and its related config mode
uint8_t steady_state(Event event, uint16_t arg);
#ifdef USE_RAMP_CONFIG
uint8_t ramp_config_state(Event event, uint16_t arg);
#endif
#ifdef USE_BATTCHECK
uint8_t battcheck_state(Event event, uint16_t arg);
#endif
#ifdef USE_THERMAL_REGULATION
#define USE_BLINK_NUM
uint8_t tempcheck_state(Event event, uint16_t arg);
uint8_t thermal_config_state(Event event, uint16_t arg);
#endif
#ifdef USE_GOODNIGHT_MODE
// 1-hour ramp down from low, then automatic off
uint8_t goodnight_state(Event event, uint16_t arg);
#endif
#ifdef USE_SOS_MODE_IN_BLINKY_GROUP
// automatic SOS emergency signal
uint8_t sos_state(Event event, uint16_t arg);
#endif
#ifdef USE_MOMENTARY_MODE
// momentary / signalling mode
uint8_t momentary_state(Event event, uint16_t arg);
uint8_t momentary_mode = 0;  // 0 = ramping, 1 = strobe
uint8_t momentary_active = 0;  // boolean, true if active *right now*
#endif

void blink_confirm(uint8_t num);
void blip();
#if defined(USE_INDICATOR_LED) && defined(TICK_DURING_STANDBY)
void indicator_blink(uint8_t arg);
#endif
#if defined(USE_AUX_RGB_LEDS) && defined(TICK_DURING_STANDBY)
uint8_t setting_rgb_mode_now = 0;
void rgb_led_update(uint8_t mode, uint8_t arg);
void rgb_led_voltage_readout(uint8_t bright);
/*
 * 0: R
 * 1: RG
 * 2:  G
 * 3:  GB
 * 4:   B
 * 5: R B
 * 6: RGB
 * 7: rainbow
 * 8: voltage
 */
const PROGMEM uint8_t rgb_led_colors[] = {
    0b00000001,  // 0: red
    0b00000101,  // 1: yellow
    0b00000100,  // 2: green
    0b00010100,  // 3: cyan
    0b00010000,  // 4: blue
    0b00010001,  // 5: purple
    0b00010101,  // 6: white
};
#define RGB_LED_NUM_COLORS 10
#define RGB_LED_NUM_PATTERNS 4
#ifndef RGB_LED_OFF_DEFAULT
//#define RGB_LED_OFF_DEFAULT 0x18  // low, voltage
#define RGB_LED_OFF_DEFAULT 0x17  // low, rainbow
#endif
#ifndef RGB_LED_LOCKOUT_DEFAULT
#define RGB_LED_LOCKOUT_DEFAULT 0x37  // blinking, rainbow
#endif
#ifndef RGB_RAINBOW_SPEED
#define RGB_RAINBOW_SPEED 0x0f  // change color every 16 frames
#endif
uint8_t rgb_led_off_mode = RGB_LED_OFF_DEFAULT;
uint8_t rgb_led_lockout_mode = RGB_LED_LOCKOUT_DEFAULT;
#endif

// remember stuff even after battery was changed
void load_config();
void save_config();
#ifdef START_AT_MEMORIZED_LEVEL
void save_config_wl();
#endif

// default ramp options if not overridden earlier per-driver
#ifndef RAMP_STYLE
#define RAMP_STYLE 0  // smooth default
#endif
#ifndef RAMP_SMOOTH_FLOOR
  #define RAMP_SMOOTH_FLOOR 1
#endif
#ifndef RAMP_SMOOTH_CEIL
  #if PWM_CHANNELS == 3
    #define RAMP_SMOOTH_CEIL MAX_Nx7135
  #else
    #define RAMP_SMOOTH_CEIL MAX_LEVEL - 30
  #endif
#endif
#ifndef RAMP_DISCRETE_FLOOR
  #define RAMP_DISCRETE_FLOOR 20
#endif
#ifndef RAMP_DISCRETE_CEIL
  #define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#endif
#ifndef RAMP_DISCRETE_STEPS
  #define RAMP_DISCRETE_STEPS 7
#endif

// mile marker(s) partway up the ramp
// default: blink only at border between regulated and FET
#ifdef BLINK_AT_RAMP_MIDDLE
  #if PWM_CHANNELS >= 3
    #ifndef BLINK_AT_RAMP_MIDDLE_1
      #define BLINK_AT_RAMP_MIDDLE_1 MAX_Nx7135
      #ifndef BLINK_AT_RAMP_MIDDLE_2
      #define BLINK_AT_RAMP_MIDDLE_2 MAX_1x7135
      #endif
    #endif
  #else
    #ifndef BLINK_AT_RAMP_MIDDLE_1
    #define BLINK_AT_RAMP_MIDDLE_1 MAX_1x7135
    #endif
  #endif
#endif

// brightness control
uint8_t memorized_level = DEFAULT_LEVEL;
#ifdef USE_MANUAL_MEMORY
uint8_t manual_memory = 0;
#endif
#ifdef USE_SIMPLE_UI
// whether to enable the simplified interface or not
uint8_t simple_ui_active = DEFAULT_SIMPLE_UI_ACTIVE;
#endif
// smooth vs discrete ramping
uint8_t ramp_style = RAMP_STYLE;  // 0 = smooth, 1 = discrete
// current values, regardless of style
uint8_t ramp_floor = RAMP_SMOOTH_FLOOR;
uint8_t ramp_ceil = RAMP_SMOOTH_CEIL;
// per style
uint8_t ramp_floors[] = {
    RAMP_SMOOTH_FLOOR,
    RAMP_DISCRETE_FLOOR,
    #ifdef USE_SIMPLE_UI
    DEFAULT_SIMPLE_UI_FLOOR,
    #endif
    };
uint8_t ramp_ceils[] = {
    RAMP_SMOOTH_CEIL,
    RAMP_DISCRETE_CEIL,
    #ifdef USE_SIMPLE_UI
    DEFAULT_SIMPLE_UI_CEIL,
    #endif
    };
uint8_t ramp_stepss[] = {
    0,
    RAMP_DISCRETE_STEPS,
    #ifdef USE_SIMPLE_UI
    DEFAULT_SIMPLE_UI_STEPS,
    #endif
    };
uint8_t ramp_discrete_step_size;  // don't set this

#ifdef USE_INDICATOR_LED
    // bits 2-3 control lockout mode
    // bits 0-1 control "off" mode
    // modes are: 0=off, 1=low, 2=high, 3=blinking (if TICK_DURING_STANDBY enabled)
    #ifdef INDICATOR_LED_DEFAULT_MODE
    uint8_t indicator_led_mode = INDICATOR_LED_DEFAULT_MODE;
    #else
        #ifdef USE_INDICATOR_LED_WHILE_RAMPING
        //uint8_t indicator_led_mode = (1<<2) + 2;
        uint8_t indicator_led_mode = (2<<2) + 1;
        #else
        uint8_t indicator_led_mode = (3<<2) + 1;
        #endif
    #endif
#endif

// calculate the nearest ramp level which would be valid at the moment
// (is a no-op for smooth ramp, but limits discrete ramp to only the
// correct levels for the user's config)
uint8_t nearest_level(int16_t target);
// ensure ramp globals are correct
void ramp_update_config();

#ifdef USE_THERMAL_REGULATION
// brightness before thermal step-down
uint8_t target_level = 0;
void set_level_and_therm_target(uint8_t level);
#else
#define set_level_and_therm_target(level) set_level(level)
#endif

#ifdef USE_VERSION_CHECK
#define USE_BLINK_DIGIT
#include "version.h"
const PROGMEM uint8_t version_number[] = VERSION_NUMBER;
uint8_t version_check_state(Event event, uint16_t arg);
#endif

#ifdef USE_RAMP_CONFIG
void ramp_config_save();
#endif
#ifdef USE_THERMAL_REGULATION
void thermal_config_save();
#endif

////////// Include all the extra sources //////////
// (is a bit weird to do things this way,
//  but it saves a lot of space by letting use use the -fwhole-program flag)

#include "config-mode.c"
#include "lockout.c"

#ifdef USE_TINT_RAMPING
#include "tint-ramping.c"
#endif

#ifdef USE_FACTORY_RESET
#include "factory-reset.c"
#endif

#ifdef USE_BEACON_MODE
#include "beacon.c"
#endif

#ifdef USE_STROBE_STATE
#include "strobes.c"
#endif


uint8_t off_state(Event event, uint16_t arg) {
    // turn emitter off when entering state
    if (event == EV_enter_state) {
        set_level(0);
        #ifdef USE_INDICATOR_LED
        indicator_led(indicator_led_mode & 0x03);
        #elif defined(USE_AUX_RGB_LEDS)
        rgb_led_update(rgb_led_off_mode, 0);
        #endif
        // sleep while off  (lower power use)
        // (unless delay requested; give the ADC some time to catch up)
        if (! arg) { go_to_standby = 1; }
        return MISCHIEF_MANAGED;
    }
    // go back to sleep eventually if we got bumped but didn't leave "off" state
    else if (event == EV_tick) {
        if (arg > HOLD_TIMEOUT) {
            go_to_standby = 1;
            #ifdef USE_INDICATOR_LED
            indicator_led(indicator_led_mode & 0x03);
            #elif defined(USE_AUX_RGB_LEDS)
            rgb_led_update(rgb_led_off_mode, arg);
            #endif
        }
        return MISCHIEF_MANAGED;
    }
    #if defined(TICK_DURING_STANDBY) && (defined(USE_INDICATOR_LED) || defined(USE_AUX_RGB_LEDS))
    // blink the indicator LED, maybe
    else if (event == EV_sleep_tick) {
        #ifdef USE_INDICATOR_LED
        if ((indicator_led_mode & 0b00000011) == 0b00000011) {
            indicator_blink(arg);
        }
        #elif defined(USE_AUX_RGB_LEDS)
        rgb_led_update(rgb_led_off_mode, arg);
        #endif
        return MISCHIEF_MANAGED;
    }
    #endif
    #if (B_TIMING_ON == B_PRESS_T)
    // hold (initially): go to lowest level (floor), but allow abort for regular click
    else if (event == EV_click1_press) {
        set_level(nearest_level(1));
        return MISCHIEF_MANAGED;
    }
    #endif  // B_TIMING_ON == B_PRESS_T
    // hold: go to lowest level
    else if (event == EV_click1_hold) {
        #if (B_TIMING_ON == B_PRESS_T)
        #ifdef MOON_TIMING_HINT
        if (arg == 0) {
            // let the user know they can let go now to stay at moon
            blip();
        } else
        #endif
        #else  // B_RELEASE_T or B_TIMEOUT_T
        set_level(nearest_level(1));
        #endif
        // don't start ramping immediately;
        // give the user time to release at moon level
        //if (arg >= HOLD_TIMEOUT) {  // smaller
        if (arg >= (!ramp_style) * HOLD_TIMEOUT) {  // more consistent
            set_state(steady_state, 1);
        }
        return MISCHIEF_MANAGED;
    }
    // hold, release quickly: go to lowest level (floor)
    else if (event == EV_click1_hold_release) {
        set_state(steady_state, 1);
        return MISCHIEF_MANAGED;
    }
    #if (B_TIMING_ON != B_TIMEOUT_T)
    // 1 click (before timeout): go to memorized level, but allow abort for double click
    else if (event == EV_click1_release) {
        #ifdef USE_MANUAL_MEMORY
        if (manual_memory)
            set_level(nearest_level(manual_memory));
        else
        #endif
        set_level(nearest_level(memorized_level));
        return MISCHIEF_MANAGED;
    }
    #endif  // if (B_TIMING_ON != B_TIMEOUT_T)
    // 1 click: regular mode
    else if (event == EV_1click) {
        #ifdef USE_MANUAL_MEMORY
        if (manual_memory)
            set_state(steady_state, manual_memory);
        else
        #endif
        set_state(steady_state, memorized_level);
        return MISCHIEF_MANAGED;
    }
    // click, hold: go to highest level (ceiling) (for ramping down)
    else if (event == EV_click2_hold) {
        set_state(steady_state, MAX_LEVEL);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: highest mode (ceiling)
    else if (event == EV_2clicks) {
        set_state(steady_state, MAX_LEVEL);
        return MISCHIEF_MANAGED;
    }
    // 3 clicks (initial press): off, to prep for later events
    else if (event == EV_click3_press) {
        set_level(0);
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_BATTCHECK
    // 3 clicks: battcheck mode / blinky mode group 1
    else if (event == EV_3clicks) {
        set_state(battcheck_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif
    // 4 clicks: soft lockout
    else if (event == EV_4clicks) {
        blink_confirm(2);
        set_state(lockout_state, 0);
        return MISCHIEF_MANAGED;
    }
    #if defined(USE_FACTORY_RESET) && defined(USE_SOFT_FACTORY_RESET)
    // 13 clicks and hold the last click: invoke factory reset (reboot)
    else if (event == EV_click13_hold) {
        reboot();
        return MISCHIEF_MANAGED;
    }
    #endif
    #ifdef USE_VERSION_CHECK
    // 15+ clicks: show the version number
    else if (event == EV_15clicks) {
        set_state(version_check_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif

    #ifdef USE_SIMPLE_UI
    // 8 clicks, but hold last click: turn simple UI off
    else if ((event == EV_click8_hold) && (!arg)) {
        blink_confirm(1);
        simple_ui_active = 0;
        return MISCHIEF_MANAGED;
    }

    ////////// Every action below here is blocked in the simple UI //////////
    if (simple_ui_active) {
        return EVENT_NOT_HANDLED;
    }
    // 8 clicks: enable simple UI
    else if (event == EV_8clicks) {
        blink_confirm(1);
        simple_ui_active = 1;
        return MISCHIEF_MANAGED;
    }
    #endif

    // click, click, long-click: strobe mode
    #ifdef USE_STROBE_STATE
    else if (event == EV_click3_hold) {
        set_state(strobe_state, 0);
        return MISCHIEF_MANAGED;
    }
    #elif defined(USE_BORING_STROBE_STATE)
    else if (event == EV_click3_hold) {
        set_state(boring_strobe_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif
    #ifdef USE_MOMENTARY_MODE
    // 5 clicks: momentary mode
    else if (event == EV_5clicks) {
        blink_confirm(1);
        set_state(momentary_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif
    #ifdef USE_INDICATOR_LED
    // 7 clicks: change indicator LED mode
    else if (event == EV_7clicks) {
        uint8_t mode = (indicator_led_mode & 3) + 1;
        #ifdef TICK_DURING_STANDBY
        mode = mode & 3;
        #else
        mode = mode % 3;
        #endif
        #ifdef INDICATOR_LED_SKIP_LOW
        if (mode == 1) { mode ++; }
        #endif
        indicator_led_mode = (indicator_led_mode & 0b11111100) | mode;
        indicator_led(mode);
        save_config();
        return MISCHIEF_MANAGED;
    }
    #elif defined(USE_AUX_RGB_LEDS)
    // 7 clicks: change RGB aux LED pattern
    else if (event == EV_7clicks) {
        uint8_t mode = (rgb_led_off_mode >> 4) + 1;
        mode = mode % RGB_LED_NUM_PATTERNS;
        rgb_led_off_mode = (mode << 4) | (rgb_led_off_mode & 0x0f);
        rgb_led_update(rgb_led_off_mode, 0);
        save_config();
        blink_confirm(1);
        return MISCHIEF_MANAGED;
    }
    // 7 clicks (hold last): change RGB aux LED color
    else if (event == EV_click7_hold) {
        setting_rgb_mode_now = 1;
        if (0 == (arg & 0x3f)) {
            uint8_t mode = (rgb_led_off_mode & 0x0f) + 1;
            mode = mode % RGB_LED_NUM_COLORS;
            rgb_led_off_mode = mode | (rgb_led_off_mode & 0xf0);
            //save_config();
        }
        rgb_led_update(rgb_led_off_mode, arg);
        return MISCHIEF_MANAGED;
    }
    else if (event == EV_click7_hold_release) {
        setting_rgb_mode_now = 0;
        save_config();
        return MISCHIEF_MANAGED;
    }
    #endif  // end 7 clicks
    #if defined(USE_TENCLICK_THERMAL_CONFIG) && defined(USE_THERMAL_REGULATION)
    // 10 clicks: thermal config mode
    else if (event == EV_10clicks) {
        push_state(thermal_config_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}


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
        // if the button is still stuck, lock the light
        else if ((arg > TICKS_PER_SECOND * 10) && (actual_level <= mode_min)) {
            blip();
            set_state(lockout_state, 0);
        }
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
        return MISCHIEF_MANAGED;
    }
    #ifdef START_AT_MEMORIZED_LEVEL
    // click, release, hold, release: save new ramp level (if necessary)
    else if (event == EV_click2_hold_release) {
        save_config_wl();
        return MISCHIEF_MANAGED;
    }
    #endif

    #if defined(USE_SET_LEVEL_GRADUALLY) || defined(USE_REVERSING)
    else if (event == EV_tick) {
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
    #endif
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


#ifdef USE_SOS_MODE
#ifdef USE_SOS_MODE_IN_BLINKY_GROUP
uint8_t sos_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: next mode
    else if (event == EV_2clicks) {
        #ifdef USE_THERMAL_REGULATION
        set_state(tempcheck_state, 0);
        #else
        set_state(battcheck_state, 0);
        #endif
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}
#endif

void sos_blink(uint8_t num, uint8_t dah) {
    #define DIT_LENGTH 200
    for (; num > 0; num--) {
        set_level(memorized_level);
        nice_delay_ms(DIT_LENGTH);
        if (dah) {  // dah is 3X as long as a dit
            nice_delay_ms(DIT_LENGTH*2);
        }
        set_level(0);
        // one "off" dit between blinks
        nice_delay_ms(DIT_LENGTH);
    }
    // three "off" dits (or one "dah") between letters
    // (except for SOS, which is collectively treated as a single "letter")
    //nice_delay_ms(DIT_LENGTH*2);
}

inline void sos_mode_iter() {
    // one iteration of main loop()
    //nice_delay_ms(1000);
    sos_blink(3, 0);  // S
    sos_blink(3, 1);  // O
    sos_blink(3, 0);  // S
    nice_delay_ms(2000);
}
#endif  // #ifdef USE_SOS_MODE


#ifdef USE_BATTCHECK
uint8_t battcheck_state(Event event, uint16_t arg) {
    ////////// Every action below here is blocked in the simple UI //////////
    #ifdef USE_SIMPLE_UI
    if (simple_ui_active) {
        return EVENT_NOT_HANDLED;
    }
    #endif

    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_GOODNIGHT_MODE
    // 2 clicks: goodnight mode
    else if (event == EV_2clicks) {
        set_state(goodnight_state, 0);
        return MISCHIEF_MANAGED;
    }
    #elif defined(USE_BEACON_MODE)
    // 2 clicks: beacon mode
    else if (event == EV_2clicks) {
        set_state(beacon_state, 0);
        return MISCHIEF_MANAGED;
    }
    #elif defined(USE_THERMAL_REGULATION)
    // 2 clicks: tempcheck mode
    else if (event == EV_2clicks) {
        set_state(tempcheck_state, 0);
        return MISCHIEF_MANAGED;
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
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_BATTCHECK
    // 2 clicks: battcheck mode
    else if (event == EV_2clicks) {
        set_state(battcheck_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif
    // 7 clicks: thermal config mode
    else if (event == EV_7clicks) {
        push_state(thermal_config_state, 0);
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}
#endif


#ifdef USE_GOODNIGHT_MODE
#define GOODNIGHT_TICKS_PER_STEPDOWN (GOODNIGHT_TIME*TICKS_PER_SECOND*60L/GOODNIGHT_LEVEL)
uint8_t goodnight_state(Event event, uint16_t arg) {
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
    // 2 clicks: next mode
    else if (event == EV_2clicks) {
        #ifdef USE_BEACON_MODE
        set_state(beacon_state, 0);
        #elif defined(USE_SOS_MODE_IN_BLINKY_GROUP)
        set_state(sos_state, 0);
        #elif defined(USE_THERMAL_REGULATION)
        set_state(tempcheck_state, 0);
        #endif
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
#endif


#ifdef USE_MOMENTARY_MODE
uint8_t momentary_state(Event event, uint16_t arg) {
    // TODO: momentary strobe here?  (for light painting)

    // init strobe mode, if relevant
    #ifdef USE_STROBE_STATE
    if ((event == EV_enter_state) && (momentary_mode == 1)) {
        strobe_state(event, arg);
    }
    #endif

    // light up when the button is pressed; go dark otherwise
    // button is being held
    if ((event & (B_CLICK | B_PRESS)) == (B_CLICK | B_PRESS)) {
        momentary_active = 1;
        // 0 = ramping, 1 = strobes
        if (momentary_mode == 0) {
            set_level(memorized_level);
        }
        return MISCHIEF_MANAGED;
    }
    // button was released
    else if ((event & (B_CLICK | B_PRESS)) == (B_CLICK)) {
        momentary_active = 0;
        set_level(0);
        //go_to_standby = 1;  // sleep while light is off
        return MISCHIEF_MANAGED;
    }

    // Sleep, dammit!  (but wait a few seconds first)
    // (because standby mode uses such little power that it can interfere
    //  with exiting via tailcap loosen+tighten unless you leave power
    //  disconnected for several seconds, so we want to be awake when that
    //  happens to speed up the process)
    else if (event == EV_tick) {
        #ifdef USE_STROBE_STATE
        if (momentary_active) {
            // 0 = ramping, 1 = strobes
            if (momentary_mode == 1) {
                return strobe_state(event, arg);
            }
        }
        else {
        #endif
            if (arg > TICKS_PER_SECOND*5) {  // sleep after 5 seconds
                go_to_standby = 1;  // sleep while light is off
                // turn off lighted button
                #ifdef USE_INDICATOR_LED
                indicator_led(0);
                #elif defined(USE_AUX_RGB_LEDS)
                rgb_led_update(0, 0);
                #endif
            }
        #ifdef USE_STROBE_STATE
        }
        #endif
        return MISCHIEF_MANAGED;
    }

    return EVENT_NOT_HANDLED;
}
#endif


#ifdef USE_VERSION_CHECK
uint8_t version_check_state(Event event, uint16_t arg) {
    return EVENT_NOT_HANDLED;
}
#endif


#ifdef USE_RAMP_CONFIG
void ramp_config_save() {
    // parse values
    uint8_t val;
    uint8_t style = ramp_style;
    // TODO: detect if we're configuring the simple UI

    val = config_state_values[0];
    if (val) { ramp_floors[style] = val; }

    val = config_state_values[1];
    if (val) { ramp_ceils[style] = MAX_LEVEL + 1 - val; }

    if (ramp_style) {  // discrete / stepped ramp
        val = config_state_values[2];
        if (val) ramp_stepss[style] = val;
    }
}

uint8_t ramp_config_state(Event event, uint16_t arg) {
    uint8_t num_config_steps;
    num_config_steps = 2 + ramp_style;
    return config_state_base(event, arg,
                             num_config_steps, ramp_config_save);
}
#endif  // #ifdef USE_RAMP_CONFIG


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
#endif  // #ifdef USE_THERMAL_REGULATION


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
#endif

void blink_confirm(uint8_t num) {
    for (; num>0; num--) {
        set_level(MAX_LEVEL/4);
        delay_4ms(10/4);
        set_level(0);
        // TODO: only do this delay if num > 1
        delay_4ms(100/4);
    }
}

// Just go dark for a moment to indicate to user that something happened
void blip() {
    uint8_t temp = actual_level;
    set_level(0);
    delay_4ms(3);
    set_level(temp);
}


#if defined(USE_INDICATOR_LED) && defined(TICK_DURING_STANDBY)
// beacon-like mode for the indicator LED
void indicator_blink(uint8_t arg) {
    // turn off aux LEDs when battery is empty
    if (voltage < VOLTAGE_LOW) { indicator_led(0); return; }

    #ifdef USE_FANCIER_BLINKING_INDICATOR

    // fancy blink, set off/low/high levels here:
    uint8_t seq[] = {0, 1, 2, 1,  0, 0, 0, 0,
                     0, 0, 1, 0,  0, 0, 0, 0};
    indicator_led(seq[arg & 15]);

    #else  // basic blink, 1/8th duty cycle

    if (! (arg & 7)) {
        indicator_led(2);
    }
    else {
        indicator_led(0);
    }

    #endif
}
#endif

#if defined(USE_AUX_RGB_LEDS) && defined(TICK_DURING_STANDBY)
uint8_t voltage_to_rgb() {
    uint8_t levels[] = {
    // voltage, color
          0, 0, // 0, R
         33, 1, // 1, R+G
         35, 2, // 2,   G
         37, 3, // 3,   G+B
         39, 4, // 4,     B
         41, 5, // 5, R + B
         44, 6, // 6, R+G+B  // skip; looks too similar to G+B
        255, 6, // 7, R+G+B
    };
    uint8_t volts = voltage;
    if (volts < 29) return 0;

    uint8_t i;
    for (i = 0;  volts >= levels[i];  i += 2) {}
    uint8_t color_num = levels[(i - 2) + 1];
    return pgm_read_byte(rgb_led_colors + color_num);
}

// do fancy stuff with the RGB aux LEDs
// mode: 0bPPPPCCCC where PPPP is the pattern and CCCC is the color
// arg: time slice number
void rgb_led_update(uint8_t mode, uint8_t arg) {
    static uint8_t rainbow = 0;  // track state of rainbow mode
    static uint8_t frame = 0;  // track state of animation mode

    // turn off aux LEDs when battery is empty
    // (but if voltage==0, that means we just booted and don't know yet)
    uint8_t volts = voltage;  // save a few bytes by caching volatile value
    if ((volts) && (volts < VOLTAGE_LOW)) {
        rgb_led_set(0);
        #ifdef USE_BUTTON_LED
        button_led_set(0);
        #endif
        return;
    }

    uint8_t pattern = (mode>>4);  // off, low, high, blinking, ... more?
    uint8_t color = mode & 0x0f;

    // preview in blinking mode is awkward... use high instead
    if ((! go_to_standby) && (pattern > 2)) { pattern = 2; }


    const uint8_t *colors = rgb_led_colors;
    uint8_t actual_color = 0;
    if (color < 7) {  // normal color
        actual_color = pgm_read_byte(colors + color);
    }
    else if (color == 7) {  // rainbow
        uint8_t speed = 0x03;  // awake speed
        if (go_to_standby) speed = RGB_RAINBOW_SPEED;  // asleep speed
        if (0 == (arg & speed)) {
            rainbow = (rainbow + 1) % 6;
        }
        actual_color = pgm_read_byte(colors + rainbow);
    }
    else {  // voltage
        // show actual voltage while asleep...
        if (go_to_standby) {
            actual_color = voltage_to_rgb();
            // choose a color based on battery voltage
            //if (volts >= 38) actual_color = pgm_read_byte(colors + 4);
            //else if (volts >= 33) actual_color = pgm_read_byte(colors + 2);
            //else actual_color = pgm_read_byte(colors + 0);
        }
        // ... but during preview, cycle colors quickly
        else {
            actual_color = pgm_read_byte(colors + (((arg>>1) % 3) << 1));
        }
    }

    // pick a brightness from the animation sequence
    if (pattern == 3) {
        // uses an odd length to avoid lining up with rainbow loop
        uint8_t animation[] = {2, 1, 0, 0,  0, 0, 0, 0,  0,
                               1, 0, 0, 0,  0, 0, 0, 0,  0, 1};
        frame = (frame + 1) % sizeof(animation);
        pattern = animation[frame];
    }
    uint8_t result;
    #ifdef USE_BUTTON_LED
    uint8_t button_led_result;
    #endif
    switch (pattern) {
        case 0:  // off
            result = 0;
            #ifdef USE_BUTTON_LED
            button_led_result = 0;
            #endif
            break;
        case 1:  // low
            result = actual_color;
            #ifdef USE_BUTTON_LED
            button_led_result = 1;
            #endif
            break;
        default:  // high
            result = (actual_color << 1);
            #ifdef USE_BUTTON_LED
            button_led_result = 2;
            #endif
            break;
    }
    rgb_led_set(result);
    #ifdef USE_BUTTON_LED
    button_led_set(button_led_result);
    #endif
}

void rgb_led_voltage_readout(uint8_t bright) {
    uint8_t color = voltage_to_rgb();
    if (bright) color = color << 1;
    rgb_led_set(color);
}
#endif


void load_config() {
    if (load_eeprom()) {
        ramp_style = eeprom[ramp_style_e];
        #ifdef USE_RAMP_CONFIG
        ramp_floors[0] = eeprom[ramp_smooth_floor_e];
        ramp_ceils[0] = eeprom[ramp_smooth_ceil_e];
        ramp_floors[1] = eeprom[ramp_discrete_floor_e];
        ramp_ceils[1] = eeprom[ramp_discrete_ceil_e];
        ramp_stepss[1] = eeprom[ramp_discrete_steps_e];
        #endif
        #ifdef USE_MANUAL_MEMORY
        manual_memory = eeprom[manual_memory_e];
        #endif
        #ifdef USE_TINT_RAMPING
        tint = eeprom[tint_e];
        #endif
        #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        strobe_type = eeprom[strobe_type_e];  // TODO: move this to eeprom_wl?
        strobe_delays[0] = eeprom[strobe_delays_0_e];
        strobe_delays[1] = eeprom[strobe_delays_1_e];
        #endif
        #ifdef USE_BIKE_FLASHER_MODE
        bike_flasher_brightness = eeprom[bike_flasher_brightness_e];
        #endif
        #ifdef USE_BEACON_MODE
        beacon_seconds = eeprom[beacon_seconds_e];
        #endif
        #ifdef USE_SIMPLE_UI
        simple_ui_active = eeprom[simple_ui_active_e];
        ramp_floors[2] = eeprom[simple_ui_floor_e];
        ramp_ceils[2] = eeprom[simple_ui_ceil_e];
        ramp_stepss[2] = eeprom[simple_ui_steps_e];
        #endif
        #ifdef USE_THERMAL_REGULATION
        therm_ceil = eeprom[therm_ceil_e];
        therm_cal_offset = eeprom[therm_cal_offset_e];
        #endif
        #ifdef USE_INDICATOR_LED
        indicator_led_mode = eeprom[indicator_led_mode_e];
        #endif
        #ifdef USE_AUX_RGB_LEDS
        rgb_led_off_mode = eeprom[rgb_led_off_mode_e];
        rgb_led_lockout_mode = eeprom[rgb_led_lockout_mode_e];
        #endif
    }
    #ifdef START_AT_MEMORIZED_LEVEL
    if (load_eeprom_wl()) {
        memorized_level = eeprom_wl[0];
    }
    #endif
}

void save_config() {
    eeprom[ramp_style_e] = ramp_style;
    #ifdef USE_RAMP_CONFIG
    eeprom[ramp_smooth_floor_e] = ramp_floors[0];
    eeprom[ramp_smooth_ceil_e] = ramp_ceils[0];
    eeprom[ramp_discrete_floor_e] = ramp_floors[1];
    eeprom[ramp_discrete_ceil_e] = ramp_ceils[1];
    eeprom[ramp_discrete_steps_e] = ramp_stepss[1];
    #endif
    #ifdef USE_MANUAL_MEMORY
    eeprom[manual_memory_e] = manual_memory;
    #endif
    #ifdef USE_TINT_RAMPING
    eeprom[tint_e] = tint;
    #endif
    #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
    eeprom[strobe_type_e] = strobe_type;  // TODO: move this to eeprom_wl?
    eeprom[strobe_delays_0_e] = strobe_delays[0];
    eeprom[strobe_delays_1_e] = strobe_delays[1];
    #endif
    #ifdef USE_BIKE_FLASHER_MODE
    eeprom[bike_flasher_brightness_e] = bike_flasher_brightness;
    #endif
    #ifdef USE_BEACON_MODE
    eeprom[beacon_seconds_e] = beacon_seconds;
    #endif
    #ifdef USE_SIMPLE_UI
    eeprom[simple_ui_active_e] = simple_ui_active;
    eeprom[simple_ui_floor_e] = ramp_floors[2];
    eeprom[simple_ui_ceil_e] = ramp_ceils[2];
    eeprom[simple_ui_steps_e] = ramp_stepss[2];
    #endif
    #ifdef USE_THERMAL_REGULATION
    eeprom[therm_ceil_e] = therm_ceil;
    eeprom[therm_cal_offset_e] = therm_cal_offset;
    #endif
    #ifdef USE_INDICATOR_LED
    eeprom[indicator_led_mode_e] = indicator_led_mode;
    #endif
    #ifdef USE_AUX_RGB_LEDS
    eeprom[rgb_led_off_mode_e] = rgb_led_off_mode;
    eeprom[rgb_led_lockout_mode_e] = rgb_led_lockout_mode;
    #endif

    save_eeprom();
}

#ifdef START_AT_MEMORIZED_LEVEL
void save_config_wl() {
    eeprom_wl[0] = memorized_level;
    save_eeprom_wl();
}
#endif


void low_voltage() {
    StatePtr state = current_state;

    // TODO: turn off aux LED(s) when power is really low

    if (0) {}  // placeholder

    #ifdef USE_STROBE_STATE
    // "step down" from strobe to something low
    else if (state == strobe_state) {
        set_state(steady_state, RAMP_SIZE/6);
    }
    #endif

    // in normal mode, step down or turn off
    else if (state == steady_state) {
        if (actual_level > 1) {
            uint8_t lvl = (actual_level >> 1) + (actual_level >> 2);
            set_level_and_therm_target(lvl);
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
    #ifdef START_AT_MEMORIZED_LEVEL
    // dual switch: e-switch + power clicky
    // power clicky acts as a momentary mode
    load_config();

    if (button_is_pressed())
        // hold button to go to moon
        push_state(steady_state, 1);
    else
        // otherwise use memory
        push_state(steady_state, memorized_level);

    #else  // if not START_AT_MEMORIZED_LEVEL

    // blink at power-on to let user know power is connected
    set_level(RAMP_SIZE/8);
    delay_4ms(3);
    set_level(0);

    #ifdef USE_FACTORY_RESET
    if (button_is_pressed())
        factory_reset();
    #endif

    load_config();

    #ifdef USE_TINT_RAMPING
    // add tint ramping underneath every other state
    push_state(tint_ramping_state, 0);
    #endif  // ifdef USE_TINT_RAMPING

    push_state(off_state, 1);

    #endif  // ifdef START_AT_MEMORIZED_LEVEL
}


void loop() {

    StatePtr state = current_state;

    #ifdef USE_AUX_RGB_LEDS_WHILE_ON
        if (! setting_rgb_mode_now) rgb_led_voltage_readout(1);
    #endif

    if (0) {}

    #ifdef USE_VERSION_CHECK
    else if (state == version_check_state) {
        for (uint8_t i=0; i<sizeof(version_number)-1; i++) {
            blink_digit(pgm_read_byte(version_number + i) - '0');
            nice_delay_ms(300);
        }
        // FIXME: when user interrupts with button, "off" takes an extra click
        //  before it'll turn back on, because the click to cancel gets sent
        //  to the "off" state instead of version_check_state
        //while (button_is_pressed()) {}
        //empty_event_sequence();

        set_state(off_state, 0);
    }
    #endif  // #ifdef USE_VERSION_CHECK

    #ifdef USE_STROBE_STATE
    else if ((state == strobe_state)
         #ifdef USE_MOMENTARY_MODE
         // also handle momentary strobes
         ||  ((state == momentary_state) && (momentary_mode == 1) && (momentary_active))
         #endif
         ) {
        uint8_t st = strobe_type;

        switch(st) {
            #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
            #ifdef USE_PARTY_STROBE_MODE
            case party_strobe_e:
            #endif
            #ifdef USE_TACTICAL_STROBE_MODE
            case tactical_strobe_e:
            #endif
                party_tactical_strobe_mode_iter(st);
                break;
            #endif

            #ifdef USE_LIGHTNING_MODE
            case lightning_storm_e:
                lightning_storm_iter();
                break;
            #endif

            #ifdef USE_BIKE_FLASHER_MODE
            case bike_flasher_e:
                bike_flasher_iter();
                break;
            #endif
        }

    }
    #endif  // #ifdef USE_STROBE_STATE

    #ifdef USE_BORING_STROBE_STATE
    else if (state == boring_strobe_state) {
        switch(boring_strobe_type) {
            #ifdef USE_POLICE_STROBE_MODE
            case 0: // police strobe
                police_strobe_iter();
                break;
            #endif

            #ifdef USE_SOS_MODE_IN_FF_GROUP
            default: // SOS
                sos_mode_iter();
                break;
            #endif
        }
    }
    #endif  // #ifdef USE_BORING_STROBE_STATE

    #ifdef USE_BATTCHECK
    else if (state == battcheck_state) {
        battcheck();
        #ifdef USE_SIMPLE_UI
        // in simple mode, turn off after one readout
        if (simple_ui_active) set_state(off_state, 0);
        #endif
    }
    #endif

    #ifdef USE_BEACON_MODE
    else if (state == beacon_state) {
        beacon_mode_iter();
    }
    #endif

    #ifdef USE_SOS_MODE_IN_BLINKY_GROUP
    else if (state == sos_state) {
        sos_mode_iter();
    }
    #endif

    #ifdef USE_THERMAL_REGULATION
    // TODO: blink out therm_ceil during thermal_config_state?
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
