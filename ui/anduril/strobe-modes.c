// strobe-modes.c: Strobe modes for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "anduril/strobe-modes.h"

#ifdef USE_STROBE_STATE
uint8_t strobe_state(Event event, uint16_t arg) {
    static int8_t ramp_direction = 1;
    #ifdef USE_RAINBOW_MODE
    // Rainbow strobe mode manipulates the channel args to rotate through hues.
    // We need to save and restore this value to avoid changing the saved
    // channel args used in ramp mode or elsewhere.
    static uint8_t rainbow_delay_direction = 1;
    static uint8_t saved_channel_arg = 0;
    static uint8_t current_channel_arg = 0;
    #endif

    // 'st' reduces ROM size slightly
    strobe_mode_te st = current_strobe_type;

    #if defined(USE_MOMENTARY_MODE) || defined(USE_TACTICAL_MODE)
    momentary_mode = 1;  // 0 = ramping, 1 = strobes
    #endif

    #ifdef USE_CANDLE_MODE
    // pass all events to candle mode, when it's active
    // (the code is in its own pseudo-state to keep things cleaner)
    if (st == candle_mode_e) {
        candle_mode_state(event, arg);
    }
    #endif

    if (0) {}  // placeholder
    // init anything which needs to be initialized
    else if (event == EV_enter_state) {
        current_strobe_type = cfg.strobe_type;
        ramp_direction = 1;
        #ifdef USE_RAINBOW_MODE
        if (current_strobe_type == rainbow_mode_e) {
            saved_channel_arg = cfg.channel_mode_args[RAINBOW_MODE_CH];
        }
        #endif
        return EVENT_HANDLED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        #ifdef USE_RAINBOW_MODE
        if (current_strobe_type == rainbow_mode_e) {
            cfg.channel_mode_args[RAINBOW_MODE_CH] = saved_channel_arg;
        }
        #endif
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    // 2 clicks: rotate through strobe/flasher modes
    else if (event == EV_2clicks) {
        #ifdef USE_RAINBOW_MODE
        if (current_strobe_type == rainbow_mode_e) {
            cfg.channel_mode_args[RAINBOW_MODE_CH] = saved_channel_arg;
        }
        #endif
        current_strobe_type = cfg.strobe_type = (st + 1) % NUM_STROBES;
        save_config();
        #ifdef USE_RAINBOW_MODE
        if (current_strobe_type == rainbow_mode_e) {
            saved_channel_arg = cfg.channel_mode_args[RAINBOW_MODE_CH];
        }
        #endif
        return EVENT_HANDLED;
    }
    #if (NUM_CHANNEL_MODES > 1) && defined(USE_CHANNEL_PER_STROBE)
    // 3 clicks: rotate through channel modes for the current strobe
    else if (event == EV_3clicks) {
        #ifdef USE_RAINBOW_MODE
        // Rainbow mode only works in one channel.
        if (current_strobe_type == rainbow_mode_e) {
            return EVENT_HANDLED;
        }
        #endif
        // TODO: maybe skip aux modes?
        set_channel_mode((channel_mode + 1) % NUM_CHANNEL_MODES);
        cfg.strobe_channels[st] = channel_mode;
        save_config();
        return EVENT_HANDLED;
    }
    #endif
    // 4 clicks: rotate backward through strobe/flasher modes
    else if (event == EV_4clicks) {
        #ifdef USE_RAINBOW_MODE
        if (current_strobe_type == rainbow_mode_e) {
            cfg.channel_mode_args[RAINBOW_MODE_CH] = saved_channel_arg;
        }
        #endif
        current_strobe_type = cfg.strobe_type = (st - 1 + NUM_STROBES) % NUM_STROBES;
        save_config();
        #ifdef USE_RAINBOW_MODE
        if (current_strobe_type == rainbow_mode_e) {
            saved_channel_arg = cfg.channel_mode_args[RAINBOW_MODE_CH];
        }
        #endif
        return EVENT_HANDLED;
    }
    // hold: change speed (go faster)
    //       or change brightness (brighter)
    else if (event == EV_click1_hold) {
        if (0) {}  // placeholder

        // party / tactical strobe faster
        #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        #ifdef USE_TACTICAL_STROBE_MODE
        else if (st <= tactical_strobe_e) {
        #else
        else if (st == party_strobe_e) {
        #endif
            if ((arg & 1) == 0) {
                uint8_t d = cfg.strobe_delays[st];
                d -= ramp_direction;
                if (d < 8) d = 8;
                else if (d > 254) d = 254;
                cfg.strobe_delays[st] = d;
            }
        }
        #endif

        // lightning has no adjustments
        //else if (st == lightning_storm_e) {}

        // biking mode brighter
        #ifdef USE_BIKE_FLASHER_MODE
        else if (st == bike_flasher_e) {
            cfg.bike_flasher_brightness += ramp_direction;
            if (cfg.bike_flasher_brightness < 2) cfg.bike_flasher_brightness = 2;
            else if (cfg.bike_flasher_brightness > MAX_BIKING_LEVEL) cfg.bike_flasher_brightness = MAX_BIKING_LEVEL;
            set_level(cfg.bike_flasher_brightness);
        }
        #endif

        // rainbow mode brighter
        #ifdef USE_RAINBOW_MODE
        else if (st == rainbow_mode_e) {
            cfg.rainbow_mode_brightness += ramp_direction;
            if (cfg.rainbow_mode_brightness < 2) cfg.rainbow_mode_brightness = 2;
            else if (cfg.rainbow_mode_brightness > MAX_LEVEL) cfg.rainbow_mode_brightness = MAX_LEVEL;
            set_level(cfg.rainbow_mode_brightness);
        }
        #endif

        // rainbow party brighter
        #ifdef USE_RAINBOW_PARTY
        else if (st == rainbow_party_e) {
            cfg.rainbow_party_brightness += ramp_direction;
            if (cfg.rainbow_party_brightness < 2) cfg.rainbow_party_brightness = 2;
            else if (cfg.rainbow_party_brightness > MAX_LEVEL) cfg.rainbow_party_brightness = MAX_LEVEL;
            set_level(cfg.rainbow_party_brightness);
        }
        #endif

        return EVENT_HANDLED;
    }
    // reverse ramp direction on hold release
    // ... and save new strobe settings
    else if (event == EV_click1_hold_release) {
        ramp_direction = -ramp_direction;
        #ifdef USE_RAINBOW_MODE
        if (current_strobe_type == rainbow_mode_e) {
            current_channel_arg = cfg.channel_mode_args[RAINBOW_MODE_CH];
            cfg.channel_mode_args[RAINBOW_MODE_CH] = saved_channel_arg;
        }
        #endif
        save_config();
        #ifdef USE_RAINBOW_MODE
        if (current_strobe_type == rainbow_mode_e) {
            cfg.channel_mode_args[RAINBOW_MODE_CH] = current_channel_arg;
        }
        #endif
        return EVENT_HANDLED;
    }
    // click, hold: change speed (go slower)
    //       or change brightness (dimmer)
    else if (event == EV_click2_hold) {
        ramp_direction = 1;

        if (0) {}  // placeholder

        // party / tactical strobe slower
        #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        #ifdef USE_TACTICAL_STROBE_MODE
        else if (st <= tactical_strobe_e) {
        #else
        else if (st == party_strobe_e) {
        #endif
            if ((arg & 1) == 0) {
                if (cfg.strobe_delays[st] < 255) cfg.strobe_delays[st] ++;
            }
        }
        #endif

        // lightning has no adjustments
        //else if (st == lightning_storm_e) {}

        // biking mode dimmer
        #ifdef USE_BIKE_FLASHER_MODE
        else if (st == bike_flasher_e) {
            if (cfg.bike_flasher_brightness > 2)
                cfg.bike_flasher_brightness --;
            set_level(cfg.bike_flasher_brightness);
        }
        #endif

        // rainbow mode dimmer
        #ifdef USE_RAINBOW_MODE
        else if (st == rainbow_mode_e) {
            if (cfg.rainbow_mode_brightness > 2)
                cfg.rainbow_mode_brightness --;
            set_level(cfg.rainbow_mode_brightness);
        }
        #endif

        // rainbow party dimmer
        #ifdef USE_RAINBOW_PARTY
        else if (st == rainbow_party_e) {
            if (cfg.rainbow_party_brightness > 2)
                cfg.rainbow_party_brightness --;
            set_level(cfg.rainbow_party_brightness);
        }
        #endif

        return EVENT_HANDLED;
    }
    // release hold: save new strobe settings
    else if (event == EV_click2_hold_release) {
        #ifdef USE_RAINBOW_MODE
        if (current_strobe_type == rainbow_mode_e) {
            current_channel_arg = cfg.channel_mode_args[RAINBOW_MODE_CH];
            cfg.channel_mode_args[RAINBOW_MODE_CH] = saved_channel_arg;
        }
        #endif
        save_config();
        #ifdef USE_RAINBOW_MODE
        if (current_strobe_type == rainbow_mode_e) {
            cfg.channel_mode_args[RAINBOW_MODE_CH] = current_channel_arg;
        }
        #endif
        return EVENT_HANDLED;
    }

    #ifdef USE_RAINBOW_MODE
    else if (event == EV_click3_hold && current_strobe_type == rainbow_mode_e) {
        // Rainbow mode is manipulating the channel mode args directly, so
        // this event is not necessary for adjusting tint/hue and can be used
        // here for adjusting rainbow cycling speed. The speed can be positive
        // or negative, allowing for cycling in either direction.
        if ((arg & 1) == 0) {
            int8_t d = cfg.rainbow_mode_speed;
            d -= rainbow_delay_direction;
            if (d < -126) d = -126;
            else if (d == 0) blip();
            else if (d > 126) d = 126;
            cfg.rainbow_mode_speed = d;
        }
        return EVENT_HANDLED;
    }

    // Reverse ramp direction, and save/restore channel_mode_args (hue).
    else if (event == EV_click3_hold_release && current_strobe_type == rainbow_mode_e) {
        rainbow_delay_direction = -rainbow_delay_direction;
        current_channel_arg = cfg.channel_mode_args[RAINBOW_MODE_CH];
        cfg.channel_mode_args[RAINBOW_MODE_CH] = saved_channel_arg;
        save_config();
        cfg.channel_mode_args[RAINBOW_MODE_CH] = current_channel_arg;
        return EVENT_HANDLED;
    }
    #endif  // USE_RAINBOW_MODE

    #ifdef USE_MOMENTARY_MODE
    // 5 clicks: go to momentary mode (momentary strobe)
    else if (event == EV_5clicks) {
        set_state(momentary_state, 0);
        set_level(0);
        return EVENT_HANDLED;
    }
    #endif
    #if defined(USE_LIGHTNING_MODE) || defined(USE_CANDLE_MODE)
    // clock tick: bump the random seed
    else if (event == EV_tick) {
        // un-reverse after 1 second
        if (arg == AUTO_REVERSE_TIME) ramp_direction = 1;

        pseudo_rand_seed += arg;
        return EVENT_HANDLED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}

// runs repeatedly in FSM loop() whenever UI is in strobe_state or momentary strobe
inline void strobe_state_iter() {
    uint8_t st = current_strobe_type;  // can't use switch() on an enum

    #if (NUM_CHANNEL_MODES > 1) && defined(USE_CHANNEL_PER_STROBE)
        // remember channel mode for each strobe
        #ifdef USE_RAINBOW_MODE
        if (st == rainbow_mode_e) {
            channel_mode = RAINBOW_MODE_CH;
        } else {
        #endif
            channel_mode = cfg.strobe_channels[st];
        #ifdef USE_RAINBOW_MODE
        }
        #endif
    #endif

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

        #ifdef USE_POLICE_COLOR_STROBE_MODE
        case police_color_strobe_e:
            police_color_strobe_iter();
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

        #ifdef USE_RAINBOW_MODE
        case rainbow_mode_e:
            rainbow_mode_iter();
            break;
        #endif

        #ifdef USE_RAINBOW_PARTY
        case rainbow_party_e:
            rainbow_party_iter();
            break;
        #endif
    }
}
#endif  // ifdef USE_STROBE_STATE

#if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
inline void party_tactical_strobe_mode_iter(uint8_t st) {
    // one iteration of main loop()
    uint8_t del = cfg.strobe_delays[st];
    // TODO: make tac strobe brightness configurable?
    set_level(STROBE_BRIGHTNESS);
    if (0) {}  // placeholder
    #ifdef USE_PARTY_STROBE_MODE
    else if (st == party_strobe_e) {  // party strobe
        #ifdef PARTY_STROBE_ONTIME
        nice_delay_ms(PARTY_STROBE_ONTIME);
        #else
        if (del < 42) delay_zero();
        else nice_delay_ms(1);
        #endif
    }
    #endif
    #ifdef USE_TACTICAL_STROBE_MODE
    else {  //tactical strobe
        nice_delay_ms(del >> 1);
    }
    #endif
    set_level(STROBE_OFF_LEVEL);
    nice_delay_ms(del);  // no return check necessary on final delay
}
#endif

#ifdef USE_POLICE_COLOR_STROBE_MODE
inline void police_color_strobe_iter() {
    // one iteration of main loop()
    uint8_t del = 66;
    // TODO: make police strobe brightness configurable
    uint8_t bright = memorized_level;
    //uint8_t channel = channel_mode;

    for (uint8_t i=0; i<10; i++) {
        if (0 == i) set_channel_mode(POLICE_COLOR_STROBE_CH1);
        else if (5 == i) set_channel_mode(POLICE_COLOR_STROBE_CH2);
        set_level(bright);
        nice_delay_ms(del >> 1);
        set_level(STROBE_OFF_LEVEL);
        nice_delay_ms(del);
    }

    // restore the channel when done
    //set_channel_mode(channel);
    channel_mode = cfg.channel_mode;
}
#endif

#ifdef USE_LIGHTNING_MODE
inline void lightning_storm_iter() {
    // one iteration of main loop()
    int16_t brightness;
    uint16_t rand_time;

    // turn the emitter on at a random level,
    // for a random amount of time between 1ms and 32ms
    //rand_time = 1 << (pseudo_rand() % 7);
    rand_time = pseudo_rand() & 63;
    brightness = 1 << (pseudo_rand() % 7);  // 1, 2, 4, 8, 16, 32, 64
    brightness += 1 << (pseudo_rand() % 5);  // 2 to 80 now
    brightness += pseudo_rand() % brightness;  // 2 to 159 now (w/ low bias)
    if (brightness > MAX_LEVEL) brightness = MAX_LEVEL;
    set_level(brightness);
    nice_delay_ms(rand_time);

    // decrease the brightness somewhat more gradually, like lightning
    uint8_t stepdown = brightness >> 3;
    if (stepdown < 1) stepdown = 1;
    while(brightness > 1) {
        nice_delay_ms(rand_time);
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
            nice_delay_ms(rand_time);
            set_level(brightness>>1);
        }
    }

    // turn the emitter off,
    // for a random amount of time between 1ms and 8192ms
    // (with a low bias)
    rand_time = 1 << (pseudo_rand() % 13);
    rand_time += pseudo_rand() % rand_time;
    set_level(0);
    nice_delay_ms(rand_time);  // no return check necessary on final delay
}
#endif

#ifdef USE_BIKE_FLASHER_MODE
#ifndef BIKE_STROBE_ONTIME
#define BIKE_STROBE_ONTIME 0
#endif
inline void bike_flasher_iter() {
    // one iteration of main loop()
    uint8_t burst = cfg.bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    for(uint8_t i=0; i<4; i++) {
        set_level(burst);
        nice_delay_ms(5 + BIKE_STROBE_ONTIME);
        set_level(cfg.bike_flasher_brightness);
        nice_delay_ms(65);
    }
    nice_delay_ms(720);  // no return check necessary on final delay
    set_level(0);
}
#endif

#ifdef USE_RAINBOW_MODE
// Setting the delay directly creates a difficult interface where most of the
// values are very slow settings and it's hard to get anything in the very
// narrow range of fast settings. Instead, we use a table calcualted based on
// a range of desired speeds.
// ./bin/rainbow_curve.py 180.0 0.1 4 16 2.0
const uint16_t rainbow_delays[127] = {703,465,346,275,227,193,168,148,132,119,108,99,91,84,78,73,69,64,61,57,54,51,49,46,44,42,40,38,37,35,34,33,31,30,29,28,27,26,25,24,23,22,22,21,20,20,19,18,18,17,17,16,16,15,15,14,14,13,13,13,12,12,12,11,11,11,10,10,10,9,9,9,9,8,8,8,15,15,7,7,13,13,6,6,6,11,11,16,5,5,14,14,9,13,4,4,15,11,7,10,13,6,6,14,8,13,5,7,11,15,4,15,7,5,14,13,4,16,8,16,15,16,6,15,13,11,16};
const uint8_t rainbow_increments[127] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,2,2,1,1,1,2,2,3,1,1,3,3,2,3,1,1,4,3,2,3,4,2,2,5,3,5,2,3,5,7,2,8,4,3,9,9,3,13,7,15,16,19,8,23,23,23,41};
inline void rainbow_mode_iter() {
    int8_t speed = cfg.rainbow_mode_speed;
    speed = speed < 0 ? -speed : speed;

    uint16_t delay = rainbow_delays[speed];
    uint8_t increment = rainbow_increments[speed];
    if (cfg.rainbow_mode_speed < 0) {
        increment = -increment;
    }
    cfg.channel_mode_args[channel_mode] += increment;
    set_level(cfg.rainbow_mode_brightness);
    nice_delay_ms(delay);
}
#endif  // USE_RAINBOW_MODE

#ifdef USE_RAINBOW_PARTY
inline void rainbow_party_iter() {
    static uint8_t step = 0;
    uint8_t ch = RAINBOW_PARTY_CH1;

    switch (step) {
        case 1:
        case 3:
        case 5:
            ch = RAINBOW_PARTY_CH2;
            break;
        case 4:
        case 6:
        case 8:
            ch = RAINBOW_PARTY_CH3;
            break;
    }
    set_channel_mode(ch);
    step = (step+1) % 9;
    set_level(cfg.rainbow_party_brightness);
    nice_delay_ms(80);
}
#endif  // USE_RAINBOW_PARTY

#ifdef USE_CANDLE_MODE
#include "anduril/candle-mode.c"
#endif


#ifdef USE_BORING_STROBE_STATE
#include "anduril/ff-strobe-modes.c"
#endif
