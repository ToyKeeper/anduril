// smooth-povd.c: Smooth post-off voltage display
// Copyright (C) 2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/smooth-povd.h"

#ifdef USE_SMOOTH_POVD

//#define povd_brightness  RAMP_SIZE
#define MS_PER_TICK  16  // FIXME: this should be defined elsewhere

uint8_t smooth_povd_state(Event event, uint16_t arg) {
    // phase 0..15: wait before starting
    // 16 = ramp-up
    // 17 = steady
    // 18 = ramp-down
    // 19+ = done, exit
    #define smooth_povd_phase_start  (256 / MS_PER_TICK)
    static uint8_t phase = 0;
    static uint8_t  brightness;
    static uint8_t povd_brightness;
    static uint16_t ticks;

    povd_brightness = calc_smooth_povd_brightness(prev_level);

    if (event == EV_enter_state) {
        phase = 0;
        brightness = 0;
        ticks = cfg.post_off_voltage * (1000 / MS_PER_TICK);
        //ticks = cfg.post_off_voltage * (1000 / 4);
        return EVENT_HANDLED;
    }

    else if (event == EV_leave_state) {
        RGB_t color = { .r=0, .g=0, .b=0 };
        set_auxrgb_pwm(color);
        set_level(0);
        return EVENT_HANDLED;
    }

    // any button press event: abort and let event fall through
    // (also abort if animation complete)
    // (also abort if we're already asleep, like we got here from autolock)
    else if ((event & B_CLICK)
            || (event == EV_sleep_tick)
            || (phase > (2 + smooth_povd_phase_start))
            ) {
        pop_state();
        return EVENT_NOT_HANDLED;
    }

    #ifdef USE_SMOOTH_STEPS
    // wait for main LED animation to finish
    else if (smooth_steps_in_progress) return EVENT_HANDLED;
    #endif

    // clock tick: animate
    else if (event == EV_tick) {
        // force ADC into voltage mode, and update 'voltage' var
        if (adc_channel) {  // force voltage, not temperature
            adc_voltage_mode();
            return EVENT_HANDLED;
        }
        else {  // update 'voltage'
            ADC_voltage_handler();
            // speed up measurement
            // (sync to latest raw value, then lowpass until next tick)
            adc_smooth[0] = adc_raw[0];
        }

        // wait a moment before starting
        if (phase < smooth_povd_phase_start) {
            phase ++;
        }
        // ramp up
        else if ((0 + smooth_povd_phase_start) == phase) {
            // fading in
            if (povd_brightness > brightness) {
                // power-linear(ish) ascent
                // (jump by ~20% of remaining distance on each frame)
                uint8_t diff = povd_brightness - brightness;
                uint8_t this = diff / smooth_povd_speed;
                if (!this) this = 1;
                brightness += this;
            }
            else { phase ++; }
        }
        // steady / main read-out
        else if ((1 + smooth_povd_phase_start) == phase) {
            ticks --;
            if (! ticks) phase ++;
        }
        // ramp down
        else if ((2 + smooth_povd_phase_start) == phase) {
            if (brightness > 8) {
                brightness -= 8;
            }
            else {
                brightness = 0;
                phase ++;
            }
        }
        //// done
        //// (should never reach this point)
        //else {
        //    set_level_zero();
        //    pop_state();
        //}

        // draw this frame
        draw_smooth_povd(brightness);

        return EVENT_HANDLED;
    }

    return EVENT_HANDLED;
}


uint8_t calc_smooth_povd_brightness (uint8_t level) {
    uint8_t povd_brightness;
    // instead of using hard thresholds, ramp brightness down
    #ifdef USE_AUX_THRESHOLD_CONFIG
        if (level < cfg.aux_low_ramp_level) povd_brightness = 0;
        else if (level < cfg.aux_high_ramp_level) {
            povd_brightness = RAMP_SIZE
                * (level - cfg.aux_low_ramp_level)
                / (cfg.aux_high_ramp_level - cfg.aux_low_ramp_level);
        }
    #else
        if (level < POST_OFF_VOLTAGE_BRIGHTNESS) {
            povd_brightness = RAMP_SIZE
                * level
                / POST_OFF_VOLTAGE_BRIGHTNESS;
        }
    #endif
    else povd_brightness = RAMP_SIZE;
    return povd_brightness;
}

void draw_smooth_povd (uint8_t level) {
    rgb_uint_t pwm = get_level_auxrgb(level);
    RGB_t color = voltage_to_rgb_t(pwm);
    set_auxrgb_pwm(color);
}

#endif  // ifdef USE_SMOOTH_POVD

