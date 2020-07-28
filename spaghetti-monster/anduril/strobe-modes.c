/*
 * strobe-modes.c: Strobe modes for Anduril.
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

#ifndef STROBE_MODES_C
#define STROBE_MODES_C

#include "strobe-modes.h"

#ifdef USE_STROBE_STATE
uint8_t strobe_state(Event event, uint16_t arg) {
    static int8_t ramp_direction = 1;

    // 'st' reduces ROM size by avoiding access to a volatile var
    // (maybe I should just make it nonvolatile?)
    strobe_mode_te st = strobe_type;

    #ifdef USE_MOMENTARY_MODE
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
        ramp_direction = 1;
        return MISCHIEF_MANAGED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: rotate through strobe/flasher modes
    else if (event == EV_2clicks) {
        strobe_type = (st + 1) % NUM_STROBES;
        save_config();
        return MISCHIEF_MANAGED;
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
                uint8_t d = strobe_delays[st];
                d -= ramp_direction;
                if (d < 8) d = 8;
                else if (d > 254) d = 254;
                strobe_delays[st] = d;
            }
        }
        #endif

        // lightning has no adjustments
        //else if (st == lightning_storm_e) {}

        // biking mode brighter
        #ifdef USE_BIKE_FLASHER_MODE
        else if (st == bike_flasher_e) {
            bike_flasher_brightness += ramp_direction;
            if (bike_flasher_brightness < 2) bike_flasher_brightness = 2;
            else if (bike_flasher_brightness > MAX_BIKING_LEVEL) bike_flasher_brightness = MAX_BIKING_LEVEL;
            set_level(bike_flasher_brightness);
        }
        #endif

        return MISCHIEF_MANAGED;
    }
    // reverse ramp direction on hold release
    // ... and save new strobe settings
    else if (event == EV_click1_hold_release) {
        ramp_direction = -ramp_direction;
        save_config();
        return MISCHIEF_MANAGED;
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
                if (strobe_delays[st] < 255) strobe_delays[st] ++;
            }
        }
        #endif

        // lightning has no adjustments
        //else if (st == lightning_storm_e) {}

        // biking mode dimmer
        #ifdef USE_BIKE_FLASHER_MODE
        else if (st == bike_flasher_e) {
            if (bike_flasher_brightness > 2)
                bike_flasher_brightness --;
            set_level(bike_flasher_brightness);
        }
        #endif

        return MISCHIEF_MANAGED;
    }
    // release hold: save new strobe settings
    else if (event == EV_click2_hold_release) {
        save_config();
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_MOMENTARY_MODE
    // 5 clicks: go to momentary mode (momentary strobe)
    else if (event == EV_5clicks) {
        set_state(momentary_state, 0);
        set_level(0);
        return MISCHIEF_MANAGED;
    }
    #endif
    #if defined(USE_LIGHTNING_MODE) || defined(USE_CANDLE_MODE)
    // clock tick: bump the random seed
    else if (event == EV_tick) {
        // un-reverse after 1 second
        if (arg == TICKS_PER_SECOND) ramp_direction = 1;

        pseudo_rand_seed += arg;
        return MISCHIEF_MANAGED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}

// runs repeatedly in FSM loop() whenever UI is in strobe_state or momentary strobe
inline void strobe_state_iter() {
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
#endif  // ifdef USE_STROBE_STATE

#if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
inline void party_tactical_strobe_mode_iter(uint8_t st) {
    // one iteration of main loop()
    uint8_t del = strobe_delays[st];
    // TODO: make tac strobe brightness configurable?
    set_level(STROBE_BRIGHTNESS);
    if (0) {}  // placeholde0
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
    set_level(0);
    nice_delay_ms(del);  // no return check necessary on final delay
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
inline void bike_flasher_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    for(uint8_t i=0; i<4; i++) {
        set_level(burst);
        nice_delay_ms(5);
        set_level(bike_flasher_brightness);
        nice_delay_ms(65);
    }
    nice_delay_ms(720);  // no return check necessary on final delay
}
#endif

#ifdef USE_CANDLE_MODE
uint8_t candle_mode_state(Event event, uint16_t arg) {
    static int8_t ramp_direction = 1;
    #define MAX_CANDLE_LEVEL (RAMP_LENGTH-CANDLE_AMPLITUDE-15)
    static uint8_t candle_wave1 = 0;
    static uint8_t candle_wave2 = 0;
    static uint8_t candle_wave3 = 0;
    static uint8_t candle_wave2_speed = 0;
    // these should add up to 100
    #define CANDLE_WAVE1_MAXDEPTH 30
    #define CANDLE_WAVE2_MAXDEPTH 45
    #define CANDLE_WAVE3_MAXDEPTH 25
    static const uint8_t candle_wave1_depth = CANDLE_WAVE1_MAXDEPTH * CANDLE_AMPLITUDE / 100;
    static uint8_t candle_wave2_depth       = CANDLE_WAVE2_MAXDEPTH * CANDLE_AMPLITUDE / 100;
    static uint8_t candle_wave3_depth       = CANDLE_WAVE3_MAXDEPTH * CANDLE_AMPLITUDE / 100;
    static uint8_t candle_mode_brightness = 24;
    static uint8_t candle_mode_timer = 0;
    #define TICKS_PER_CANDLE_MINUTE 4096 // about 65 seconds
    #define MINUTES_PER_CANDLE_HALFHOUR 27 // ish

    if (event == EV_enter_state) {
        candle_mode_timer = 0;  // in case any time was left over from earlier
        ramp_direction = 1;
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: cancel timer
    else if (event == EV_2clicks) {
        // parent state just rotated through strobe/flasher modes,
        // so cancel timer...  in case any time was left over from earlier
        candle_mode_timer = 0;
        return MISCHIEF_MANAGED;
    }
    // hold: change brightness (brighter)
    else if (event == EV_click1_hold) {
        // ramp away from extremes
        if (! arg) {
            if (candle_mode_brightness >= MAX_CANDLE_LEVEL) { ramp_direction = -1; }
            else if (candle_mode_brightness <= 1) { ramp_direction = 1; }
        }
        // change brightness, but not too far
        candle_mode_brightness += ramp_direction;
        if (candle_mode_brightness < 1) candle_mode_brightness = 1;
        else if (candle_mode_brightness > MAX_CANDLE_LEVEL) candle_mode_brightness = MAX_CANDLE_LEVEL;
        return MISCHIEF_MANAGED;
    }
    // reverse ramp direction on hold release
    else if (event == EV_click1_hold_release) {
        ramp_direction = -ramp_direction;
        return MISCHIEF_MANAGED;
    }
    // click, hold: change brightness (dimmer)
    else if (event == EV_click2_hold) {
        ramp_direction = 1;
        if (candle_mode_brightness > 1)
            candle_mode_brightness --;
        return MISCHIEF_MANAGED;
    }
    // 3 clicks: add 30m to candle timer
    else if (event == EV_3clicks) {
        if (candle_mode_timer < (255 - MINUTES_PER_CANDLE_HALFHOUR)) {
            // add 30m to the timer
            candle_mode_timer += MINUTES_PER_CANDLE_HALFHOUR;
            // blink to confirm
            set_level(actual_level + 32);
            delay_4ms(2);
        }
        return MISCHIEF_MANAGED;
    }
    // clock tick: animate candle brightness
    else if (event == EV_tick) {
        // un-reverse after 1 second
        if (arg == TICKS_PER_SECOND) ramp_direction = 1;

        // self-timer dims the light during the final minute
        uint8_t subtract = 0;
        if (candle_mode_timer == 1) {
            subtract = ((candle_mode_brightness+CANDLE_AMPLITUDE)
                     * ((arg & (TICKS_PER_CANDLE_MINUTE-1)) >> 4))
                     >> 8;
        }
        // we passed a minute mark, decrease timer if it's running
        if ((arg & (TICKS_PER_CANDLE_MINUTE-1)) == (TICKS_PER_CANDLE_MINUTE - 1)) {
            if (candle_mode_timer > 0) {
                candle_mode_timer --;
                //set_level(0);  delay_4ms(2);
                // if the timer ran out, shut off
                if (! candle_mode_timer) {
                    set_state(off_state, 0);
                }
            }
        }
        // 3-oscillator synth for a relatively organic pattern
        uint8_t add;
        add = ((triangle_wave(candle_wave1) * candle_wave1_depth) >> 8)
            + ((triangle_wave(candle_wave2) * candle_wave2_depth) >> 8)
            + ((triangle_wave(candle_wave3) * candle_wave3_depth) >> 8);
        int8_t brightness = candle_mode_brightness + add - subtract;
        if (brightness < 0) { brightness = 0; }
        set_level(brightness);

        // wave1: slow random LFO
        // TODO: make wave slower and more erratic?
        if ((arg & 1) == 0) candle_wave1 += pseudo_rand() & 1;
        // wave2: medium-speed erratic LFO
        candle_wave2 += candle_wave2_speed;
        // wave3: erratic fast wave
        candle_wave3 += pseudo_rand() % 37;
        // S&H on wave2 frequency to make it more erratic
        if ((pseudo_rand() & 0b00111111) == 0)
            candle_wave2_speed = pseudo_rand() % 13;
        // downward sawtooth on wave2 depth to simulate stabilizing
        if ((candle_wave2_depth > 0) && ((pseudo_rand() & 0b00111111) == 0))
            candle_wave2_depth --;
        // random sawtooth retrigger
        if (pseudo_rand() == 0) {
            // random amplitude
            //candle_wave2_depth = 2 + (pseudo_rand() % ((CANDLE_WAVE2_MAXDEPTH * CANDLE_AMPLITUDE / 100) - 2));
            candle_wave2_depth = pseudo_rand() % (CANDLE_WAVE2_MAXDEPTH * CANDLE_AMPLITUDE / 100);
            //candle_wave3_depth = 5;
            candle_wave2 = 0;
        }
        // downward sawtooth on wave3 depth to simulate stabilizing
        if ((candle_wave3_depth > 2) && ((pseudo_rand() & 0b00011111) == 0))
            candle_wave3_depth --;
        if ((pseudo_rand() & 0b01111111) == 0)
            // random amplitude
            //candle_wave3_depth = 2 + (pseudo_rand() % ((CANDLE_WAVE3_MAXDEPTH * CANDLE_AMPLITUDE / 100) - 2));
            candle_wave3_depth = pseudo_rand() % (CANDLE_WAVE3_MAXDEPTH * CANDLE_AMPLITUDE / 100);
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}
#endif  // #ifdef USE_CANDLE_MODE


#ifdef USE_BORING_STROBE_STATE
#include "ff-strobe-modes.c"
#endif


#endif

