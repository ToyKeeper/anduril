/*
 * off-mode.c: "Off" mode for Anduril.
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

#ifndef OFF_MODE_C
#define OFF_MODE_C

#include "off-mode.h"

#ifdef USE_SUNSET_TIMER
#include "sunset-timer.h"
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
        #ifdef USE_SUNSET_TIMER
        sunset_timer = 0;  // needs a reset in case previous timer was aborted
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
    #if defined(TICK_DURING_STANDBY)
    // blink the indicator LED, maybe
    else if (event == EV_sleep_tick) {
        #ifdef USE_INDICATOR_LED
        if ((indicator_led_mode & 0b00000011) == 0b00000011) {
            indicator_blink(arg);
        }
        #elif defined(USE_AUX_RGB_LEDS)
        rgb_led_update(rgb_led_off_mode, arg);
        #endif

        #ifdef USE_AUTOLOCK
        // lock the light after being off for N minutes
        #ifdef USE_SIMPLE_UI
        if (! simple_ui_active) {  // no auto-lock in Simple UI
        #endif
            uint16_t ticks = autolock_time * SLEEP_TICKS_PER_MINUTE;
            if ((autolock_time > 0)  && (arg > ticks)) {
                set_state(lockout_state, 0);
            }
        #ifdef USE_SIMPLE_UI
        }
        #endif
        #endif  // ifdef USE_AUTOLOCK
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
    // click, hold: momentary at ceiling level
    else if (event == EV_click2_hold) {
        set_level(nearest_level(MAX_LEVEL));
        return MISCHIEF_MANAGED;
    }
    else if (event == EV_click2_hold_release) {
        set_level(0);
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
    #ifdef USE_LOCKOUT_MODE
    // 4 clicks: soft lockout
    else if (event == EV_4clicks) {
        blink_once();
        set_state(lockout_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif
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
    // 10 clicks, but hold last click: turn simple UI off (or configure it)
    else if ((event == EV_click10_hold) && (!arg)) {
        if (simple_ui_active) {  // turn off simple UI
            blink_once();
            simple_ui_active = 0;
            save_config();
        }
        else {  // configure simple UI ramp
            push_state(simple_ui_config_state, 0);
        }
        return MISCHIEF_MANAGED;
    }

    ////////// Every action below here is blocked in the simple UI //////////
    if (simple_ui_active) {
        return EVENT_NOT_HANDLED;
    }
    // 10 clicks: enable simple UI
    else if (event == EV_10clicks) {
        blink_once();
        simple_ui_active = 1;
        save_config();
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
        blink_once();
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
        blink_once();
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
    return EVENT_NOT_HANDLED;
}


#endif

