/*
 * DarkHorse: Improved ZebraLight clone UI for SpaghettiMonster.
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

#define FSM_EMISAR_D4_LAYOUT
#define USE_LVP
#define USE_THERMAL_REGULATION
#define DEFAULT_THERM_CEIL 45
#define USE_DELAY_4MS
#define USE_RAMPING
#define RAMP_LENGTH 150
#define USE_BATTCHECK
#define BATTCHECK_4bars
#include "spaghetti-monster.h"

// FSM states
uint8_t off_state(EventPtr event, uint16_t arg);
uint8_t low_mode_state(EventPtr event, uint16_t arg);
uint8_t med_mode_state(EventPtr event, uint16_t arg);
uint8_t hi_mode_state(EventPtr event, uint16_t arg);
//uint8_t strobe_state(EventPtr event, uint16_t arg);
//uint8_t beacon_state(EventPtr event, uint16_t arg);
#ifdef USE_BATTCHECK
uint8_t battcheck_state(EventPtr event, uint16_t arg);
#endif
// Not a FSM state, just handles stuff common to all low/med/hi states
uint8_t any_mode_state(EventPtr event, uint16_t arg, uint8_t *primary, uint8_t *secondary, uint8_t *modes);

// toggle between L1/L2, M1/M2, H1/H2
uint8_t L1 = 1;
uint8_t M1 = 1;
uint8_t H1 = 1;
// brightness for L2, M2, H2  (valid range 1 to 3 inclusive)
uint8_t L2 = 1;
uint8_t M2 = 1;
uint8_t H2 = 1;
// mode groups, ish
uint8_t low_modes[] = {12, 1, 4, 9};  // 3.3 lm, 2.0 lm, 0.8 lm, 0.3 lm
uint8_t med_modes[] = {56, 21, 29, 37};  // 101 lm, 35 lm, 20 lm, 10 lm
uint8_t hi_modes[] = {MAX_LEVEL, 81, 96, 113};  // 1500 lm, 678 lm, 430 lm, 270 lm

#ifdef USE_THERMAL_REGULATION
// brightness before thermal step-down
uint8_t target_level = 0;
#endif

void set_any_mode(uint8_t primary, uint8_t secondary, uint8_t *modes) {
    // primary (H1/M1/L1)
    if (primary) {
        set_level(modes[0]);
    }
    // secondary (H2/M2/L2)
    else {
        set_level(modes[secondary]);
    }
    #ifdef USE_THERMAL_REGULATION
    target_level = actual_level;
    #endif
}

inline void set_low_mode() { set_any_mode(L1, L2, low_modes); }
inline void set_med_mode() { set_any_mode(M1, M2, med_modes); }
inline void set_hi_mode()  { set_any_mode(H1, H2, hi_modes); }


uint8_t off_state(EventPtr event, uint16_t arg) {
    // turn emitter off when entering state
    if (event == EV_enter_state) {
        set_level(0);
        // sleep while off  (lower power use)
        standby_mode();
        return 0;
    }
    // hold (initially): go to lowest level, but allow abort for regular click
    else if (event == EV_click1_press) {
        set_low_mode();
        return 0;
    }
    // 1 click (before timeout): go to high level, but allow abort for double click
    else if (event == EV_click1_release) {
        set_hi_mode();
        return 0;
    }
    // 1 click: high mode
    else if (event == EV_1click) {
        set_state(hi_mode_state, 0);
        return 0;
    }
    // click, hold (initially): go to medium mode, but allow abort
    else if (event == EV_click2_press) {
        set_med_mode();
        return 0;
    }
    // 2 clicks: medium mode
    else if (event == EV_2clicks) {
        set_state(med_mode_state, 0);
        return 0;
    }
    // click, click, hold (initially): light off, prep for blinkies
    else if (event == EV_click3_press) {
        set_level(0);
        return 0;
    }
    // 3 clicks: strobe mode
    /* TODO: implement
    else if (event == EV_3clicks) {
        set_state(party_strobe_state, 255);
        return 0;
    }
    */
    #ifdef USE_BATTCHECK
    // 4 clicks: battcheck mode
    else if (event == EV_4clicks) {
        set_state(battcheck_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif
    // hold: go to low mode, but allow ramping up
    else if (event == EV_click1_hold) {
        // don't start ramping immediately;
        // give the user time to release at low mode
        if (arg >= HOLD_TIMEOUT)
            set_state(low_mode_state, 0);
        return 0;
    }
    // hold, release quickly: go to low mode
    else if (event == EV_click1_hold_release) {
        set_state(low_mode_state, 0);
        return 0;
    }
    /* TODO: implement
    // click-release-hold: discrete ramp through all levels
    else if (event == EV_click2_hold) {
        set_state(steady_state, MAX_LEVEL);
        return 0;
    }
    */
    return 1;
}


uint8_t any_mode_state(EventPtr event, uint16_t arg, uint8_t *primary, uint8_t *secondary, uint8_t *modes) {
    // turn on LED when entering the mode
    if (event == EV_enter_state) {
        set_any_mode(*primary, *secondary, modes);
        return MISCHIEF_MANAGED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: toggle primary/secondary level
    else if (event == EV_2clicks) {
        *primary ^= 1;
        set_any_mode(*primary, *secondary, modes);
        return MISCHIEF_MANAGED;
    }
    // click-release-hold: change secondary level
    if (event == EV_click2_hold) {
        if (arg % HOLD_TIMEOUT == 0) {
            *secondary = (*secondary + 1) & 3;
            if (! *secondary) *secondary = 1;
            *primary = 0;
            set_any_mode(*primary, *secondary, modes);
        }
        return MISCHIEF_MANAGED;
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
        return 0;
    }
    // underheating: increase slowly if we're lower than the target
    //               (proportional to how low we are)
    else if (event == EV_temperature_low) {
        if (actual_level < target_level) {
            uint8_t stepup = actual_level + (arg>>1);
            if (stepup > target_level) stepup = target_level;
            set_level(stepup);
        }
        return 0;
    }
    #endif
    return EVENT_NOT_HANDLED;
}

uint8_t low_mode_state(EventPtr event, uint16_t arg) {
    // hold: change brightness (brighter)
    if (event == EV_click1_hold) {
        if (arg % HOLD_TIMEOUT == 0) {
            set_state(med_mode_state, 0);
        }
        return MISCHIEF_MANAGED;
    }
    else return any_mode_state(event, arg, &L1, &L2, low_modes);
}

uint8_t med_mode_state(EventPtr event, uint16_t arg) {
    // hold: change brightness (brighter)
    if (event == EV_click1_hold) {
        if (arg % HOLD_TIMEOUT == 0) {
            set_state(hi_mode_state, 0);
        }
        return MISCHIEF_MANAGED;
    }
    else return any_mode_state(event, arg, &M1, &M2, med_modes);
}

uint8_t hi_mode_state(EventPtr event, uint16_t arg) {
    // hold: change brightness (brighter)
    if (event == EV_click1_hold) {
        if (arg % HOLD_TIMEOUT == 0) {
            set_state(low_mode_state, 0);
        }
        return MISCHIEF_MANAGED;
    }
    else return any_mode_state(event, arg, &H1, &H2, hi_modes);
}


#ifdef USE_BATTCHECK
uint8_t battcheck_state(EventPtr event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}
#endif


void low_voltage() {
    // TODO: rewrite this
    /*
    // "step down" from strobe to something low
    if (current_state == party_strobe_state) {
        set_state(steady_state, RAMP_SIZE/6);
    }
    */
    if (current_state == hi_mode_state) {
        set_state(med_mode_state, 0);
    }
    else if (current_state == med_mode_state) {
        set_state(low_mode_state, 0);
    }
    else if (current_state == low_mode_state) {
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
    #ifdef USE_BATTCHECK
    if (current_state == battcheck_state) {
        nice_delay_ms(500);  // wait a moment to measure voltage
        battcheck();
        set_state(off_state, 0);
    }
    #endif
}


