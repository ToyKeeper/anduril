// off-mode.c: "Off" mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/off-mode.h"

#ifdef USE_SUNSET_TIMER
#include "anduril/sunset-timer.h"
#endif

// set level smooth maybe
void off_state_set_level(uint8_t level);


uint8_t off_state(Event event, uint16_t arg) {

    // turn emitter off when entering state
    if (event == EV_enter_state) {
        // turn off
        off_state_set_level(0);
        #ifdef USE_SMOOTH_STEPS
            // don't go to sleep while animating
            arg |= smooth_steps_in_progress;
        #endif
        ticks_since_on = 0;
        #if NUM_CHANNEL_MODES > 1
            // reset to ramp mode's channel when light turns off
            channel_mode = cfg.channel_mode;
        #endif
        #ifdef USE_INDICATOR_LED
        // redundant, sleep tick does the same thing
        //indicator_led_update(cfg.indicator_led_mode & 0x03, 0);
        #elif defined(USE_AUX_RGB_LEDS)
        // redundant, sleep tick does the same thing
        //rgb_led_update(cfg.rgb_led_off_mode, 0);
        #endif
        #ifdef USE_SUNSET_TIMER
        sunset_timer = 0;  // needs a reset in case previous timer was aborted
        #endif
        // sleep while off  (lower power use)
        // (unless delay requested; give the ADC some time to catch up)
        if (! arg) { go_to_standby = 1; }
        return EVENT_HANDLED;
    }

    // go back to sleep eventually if we got bumped but didn't leave "off" state
    else if (event == EV_tick) {
        if (arg > HOLD_TIMEOUT
            #ifdef USE_SMOOTH_STEPS
                && (! smooth_steps_in_progress)
            #endif
            ) {
            go_to_standby = 1;
            #ifdef USE_INDICATOR_LED
            // redundant, sleep tick does the same thing
            //indicator_led_update(cfg.indicator_led_mode & 0x03, arg);
            #elif defined(USE_AUX_RGB_LEDS)
            // redundant, sleep tick does the same thing
            //rgb_led_update(cfg.rgb_led_off_mode, arg);
            #endif
        }
        return EVENT_HANDLED;
    }

    #if defined(TICK_DURING_STANDBY)
    // blink the indicator LED, maybe
    else if (event == EV_sleep_tick) {
        if (ticks_since_on < 255) ticks_since_on ++;
        #if defined(USE_MANUAL_MEMORY) && defined(USE_MANUAL_MEMORY_TIMER)
        // reset to manual memory level when timer expires
        if (cfg.manual_memory &&
                (arg >= (cfg.manual_memory_timer * SLEEP_TICKS_PER_MINUTE))) {
            manual_memory_restore();
        }
        #endif  // ifdef USE_MANUAL_MEMORY_TIMER
        #ifdef USE_INDICATOR_LED
        indicator_led_update(cfg.indicator_led_mode & 0x03, arg);
        #elif defined(USE_AUX_RGB_LEDS)
        rgb_led_update(cfg.rgb_led_off_mode, arg);
        #endif

        #ifdef USE_AUTOLOCK
            // lock the light after being off for N minutes
            uint16_t ticks = cfg.autolock_time * SLEEP_TICKS_PER_MINUTE;
            if ((cfg.autolock_time > 0)  && (arg > ticks)) {
                set_state(lockout_state, 0);
            }
        #endif  // ifdef USE_AUTOLOCK
        return EVENT_HANDLED;
    }
    #endif

    #if (B_TIMING_ON == B_PRESS_T)
    // hold (initially): go to lowest level (floor), but allow abort for regular click
    else if (event == EV_click1_press) {
        off_state_set_level(nearest_level(1));
        return EVENT_HANDLED;
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
        off_state_set_level(nearest_level(1));
        #endif
        #ifdef USE_RAMP_AFTER_MOON_CONFIG
        if (cfg.dont_ramp_after_moon) {
            return EVENT_HANDLED;
        }
        #endif
        // don't start ramping immediately;
        // give the user time to release at moon level
        //if (arg >= HOLD_TIMEOUT) {  // smaller
        if (arg >= (!cfg.ramp_style) * HOLD_TIMEOUT) {  // more consistent
            set_state(steady_state, 1);
        }
        return EVENT_HANDLED;
    }

    // hold, release quickly: go to lowest level (floor)
    else if (event == EV_click1_hold_release) {
        set_state(steady_state, 1);
        return EVENT_HANDLED;
    }

    #if (B_TIMING_ON != B_TIMEOUT_T)
    // 1 click (before timeout): go to memorized level, but allow abort for double click
    else if (event == EV_click1_release) {
        #if defined(USE_MANUAL_MEMORY) && !defined(USE_MANUAL_MEMORY_TIMER)
            // this clause probably isn't used by any configs any more
            // but is included just in case someone configures it this way
            if (cfg.manual_memory) {
                manual_memory_restore();
            }
        #endif
        off_state_set_level(nearest_level(memorized_level));
        return EVENT_HANDLED;
    }
    #endif  // if (B_TIMING_ON != B_TIMEOUT_T)

    // 1 click: regular mode
    else if (event == EV_1click) {
        #if (B_TIMING_ON != B_TIMEOUT_T)
        set_state(steady_state, memorized_level);
        #else
        // FIXME: B_TIMEOUT_T breaks manual_memory and manual_memory_timer
        //        (need to duplicate manual mem logic here, probably)
        set_state(steady_state, memorized_level);
        #endif
        return EVENT_HANDLED;
    }

    // click, hold: momentary at ceiling or turbo
    else if (event == EV_click2_hold) {
        ticks_since_on = 0;  // momentary turbo is definitely "on"
        uint8_t turbo_level;  // how bright is "turbo"?

        #if defined(USE_2C_STYLE_CONFIG)  // user can choose 2C behavior
            uint8_t style_2c = cfg.ramp_2c_style;
            #ifdef USE_SIMPLE_UI
            // simple UI has its own turbo config
            if (cfg.simple_ui_active) style_2c = cfg.ramp_2c_style_simple;
            #endif
            // 0  = ceiling
            // 1+ = full power
            if (0 == style_2c) turbo_level = nearest_level(MAX_LEVEL);
            else turbo_level = MAX_LEVEL;
        #else
            // simple UI: ceiling
            // full UI: full power
            #ifdef USE_SIMPLE_UI
            if (cfg.simple_ui_active) turbo_level = nearest_level(MAX_LEVEL);
            else
            #endif
            turbo_level = MAX_LEVEL;
        #endif

        off_state_set_level(turbo_level);
        return EVENT_HANDLED;
    }
    else if (event == EV_click2_hold_release) {
        off_state_set_level(0);
        return EVENT_HANDLED;
    }

    // 2 clicks: highest mode (ceiling)
    else if (event == EV_2clicks) {
        set_state(steady_state, MAX_LEVEL);
        return EVENT_HANDLED;
    }

    // 3 clicks (initial press): off, to prep for later events
    else if (event == EV_click3_press) {
        #ifdef USE_SMOOTH_STEPS
            // immediately cancel any animations in progress
            smooth_steps_in_progress = 0;
        #endif
        off_state_set_level(0);
        return EVENT_HANDLED;
    }

    #ifdef USE_BATTCHECK
    // 3 clicks: battcheck mode / blinky mode group 1
    else if (event == EV_3clicks) {
        set_state(battcheck_state, 0);
        return EVENT_HANDLED;
    }
    #endif

    #ifdef USE_LOCKOUT_MODE
    // 4 clicks: soft lockout
    else if (event == EV_4clicks) {
        blink_once();
        set_state(lockout_state, 0);
        return EVENT_HANDLED;
    }
    #endif

    #if defined(USE_FACTORY_RESET) && defined(USE_SOFT_FACTORY_RESET)
    // 13 clicks and hold the last click: invoke factory reset (reboot)
    else if (event == EV_click13_hold) {
        reboot();
        return EVENT_HANDLED;
    }
    #endif

    #ifdef USE_VERSION_CHECK
    // 15+ clicks: show the version number
    else if (event == EV_15clicks) {
        set_state(version_check_state, 0);
        return EVENT_HANDLED;
    }
    #endif

    #ifdef USE_SIMPLE_UI
    // 10 clicks, but hold last click: turn simple UI off (or configure it)
    else if ((event == EV_click10_hold) && (!arg)) {
        if (cfg.simple_ui_active) {  // turn off simple UI
            blink_once();
            cfg.simple_ui_active = 0;
            save_config();
        }
        else {  // configure simple UI ramp
            push_state(simple_ui_config_state, 0);
        }
        return EVENT_HANDLED;
    }

    ////////// Every action below here is blocked in the (non-Extended) Simple UI //////////

    #ifndef USE_EXTENDED_SIMPLE_UI
    if (cfg.simple_ui_active) {
        return EVENT_NOT_HANDLED;
    }
    #endif  // ifndef USE_EXTENDED_SIMPLE_UI
    #endif  // ifdef USE_SIMPLE_UI

    #ifdef USE_INDICATOR_LED
    // 7 clicks: change indicator LED mode
    else if (event == EV_7clicks) {
        uint8_t mode = (cfg.indicator_led_mode & 3) + 1;
        #ifdef TICK_DURING_STANDBY
        mode = mode & 3;
        #else
        mode = mode % 3;
        #endif
        #ifdef INDICATOR_LED_SKIP_LOW
        if (mode == 1) { mode ++; }
        #endif
        cfg.indicator_led_mode = (cfg.indicator_led_mode & 0b11111100) | mode;
        // redundant, sleep tick does the same thing
        //indicator_led_update(cfg.indicator_led_mode & 0x03, arg);
        save_config();
        return EVENT_HANDLED;
    }
    #elif defined(USE_AUX_RGB_LEDS)
    // 7 clicks: change RGB aux LED pattern
    else if (event == EV_7clicks) {
        uint8_t mode = (cfg.rgb_led_off_mode >> 4) + 1;
        mode = mode % RGB_LED_NUM_PATTERNS;
        cfg.rgb_led_off_mode = (mode << 4) | (cfg.rgb_led_off_mode & 0x0f);
        rgb_led_update(cfg.rgb_led_off_mode, 0);
        save_config();
        blink_once();
        return EVENT_HANDLED;
    }
    // 7 clicks (hold last): change RGB aux LED color
    else if (event == EV_click7_hold) {
        setting_rgb_mode_now = 1;
        if (0 == (arg & 0x3f)) {
            uint8_t mode = (cfg.rgb_led_off_mode & 0x0f) + 1;
            mode = mode % RGB_LED_NUM_COLORS;
            cfg.rgb_led_off_mode = mode | (cfg.rgb_led_off_mode & 0xf0);
            //save_config();
        }
        rgb_led_update(cfg.rgb_led_off_mode, arg);
        return EVENT_HANDLED;
    }
    else if (event == EV_click7_hold_release) {
        setting_rgb_mode_now = 0;
        save_config();
        return EVENT_HANDLED;
    }
    #endif  // end 7 clicks

    ////////// Every action below here is blocked in the Extended Simple UI //////////

    #ifdef USE_SIMPLE_UI
    #ifdef USE_EXTENDED_SIMPLE_UI
    if (cfg.simple_ui_active) {
        return EVENT_NOT_HANDLED;
    }
    #endif  // ifdef USE_EXTENDED_SIMPLE_UI

    // click, click, long-click: strobe mode
    #ifdef USE_STROBE_STATE
    else if (event == EV_click3_hold) {
        set_state(strobe_state, 0);
        return EVENT_HANDLED;
    }
    #elif defined(USE_BORING_STROBE_STATE)
    else if (event == EV_click3_hold) {
        set_state(boring_strobe_state, 0);
        return EVENT_HANDLED;
    }
    #endif

    // 10 clicks: enable simple UI
    else if (event == EV_10clicks) {
        blink_once();
        cfg.simple_ui_active = 1;
        save_config();
        return EVENT_HANDLED;
    }
    #endif  // ifdef USE_SIMPLE_UI

    #ifdef USE_MOMENTARY_MODE
    // 5 clicks: momentary mode
    else if (event == EV_5clicks) {
        blink_once();
        set_state(momentary_state, 0);
        return EVENT_HANDLED;
    }
    #endif

    #ifdef USE_TACTICAL_MODE
    // 6 clicks: tactical mode
    else if (event == EV_6clicks) {
        blink_once();
        set_state(tactical_state, 0);
        return EVENT_HANDLED;
    }
    #endif

    #ifdef USE_GLOBALS_CONFIG
    // 9 clicks, but hold last click: configure misc global settings
    else if ((event == EV_click9_hold) && (!arg)) {
        push_state(globals_config_state, 0);
        return EVENT_HANDLED;
    }
    #endif

    return EVENT_NOT_HANDLED;
}


void off_state_set_level(uint8_t level) {
    // this pattern gets used a few times, so reduce duplication
    #ifdef USE_SMOOTH_STEPS
        if (cfg.smooth_steps_style) set_level_smooth(level, 8);
        else
    #endif
    set_level(level);
}

