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

#include "hwdef-Emisar_D4.h"
#define USE_LVP
#define USE_THERMAL_REGULATION
#define DEFAULT_THERM_CEIL 45
#define USE_RAMPING
#define RAMP_LENGTH 150
#define USE_BATTCHECK
#define BATTCHECK_4bars
#define DONT_DELAY_AFTER_BATTCHECK
#define USE_EEPROM
#define EEPROM_BYTES 5
#include "spaghetti-monster.h"

// FSM states
uint8_t off_state(Event event, uint16_t arg);
uint8_t low_mode_state(Event event, uint16_t arg);
uint8_t med_mode_state(Event event, uint16_t arg);
uint8_t hi_mode_state(Event event, uint16_t arg);
uint8_t strobe_beacon_state(Event event, uint16_t arg);
#ifdef USE_BATTCHECK
uint8_t battcheck_state(Event event, uint16_t arg);
#endif
// Not a FSM state, just handles stuff common to all low/med/hi states
uint8_t any_mode_state(Event event, uint16_t arg, uint8_t *primary, uint8_t *secondary, uint8_t *modes);

void load_config();
void save_config();

// toggle between L1/L2, M1/M2, H1/H2
uint8_t L1 = 1;
uint8_t M1 = 1;
uint8_t H1 = 1;
// brightness for L2, M2, H2  (valid range 1 to 3 inclusive)
uint8_t L2 = 1;
uint8_t M2 = 1;
uint8_t H2 = 1;
// mode groups, ish
uint8_t low_modes[] = {12, 3, 5, 9};  // 3.3 lm, 2.0 lm, 0.8 lm, 0.3 lm
uint8_t med_modes[] = {56, 21, 29, 37};  // 101 lm, 35 lm, 20 lm, 10 lm
uint8_t hi_modes[] = {MAX_LEVEL, 81, 96, 113};  // 1500 lm, 678 lm, 430 lm, 270 lm
// strobe/beacon modes:
// 0: 0.2 Hz beacon at L1
// 1: 0.2 Hz beacon at H1
// 2: 4 Hz strobe at H1
// 3: 19 Hz strobe at H1
uint8_t strobe_beacon_mode = 0;

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


uint8_t off_state(Event event, uint16_t arg) {
    // turn emitter off when entering state
    if (event == EV_enter_state) {
        set_level(0);
        // sleep while off  (lower power use)
        go_to_standby = 1;
        return EVENT_HANDLED;
    }
    // hold (initially): go to lowest level, but allow abort for regular click
    else if (event == EV_click1_press) {
        set_low_mode();
        return EVENT_HANDLED;
    }
    // 1 click (before timeout): go to high level, but allow abort for double click
    else if (event == EV_click1_release) {
        set_hi_mode();
        return EVENT_HANDLED;
    }
    // 1 click: high mode
    else if (event == EV_1click) {
        set_state(hi_mode_state, 0);
        return EVENT_HANDLED;
    }
    // click, press (initially): go to medium mode, but allow abort
    else if (event == EV_click2_press) {
        set_med_mode();
        return EVENT_HANDLED;
    }
    // 2 clicks: medium mode
    else if (event == EV_2clicks) {
        set_state(med_mode_state, 0);
        return EVENT_HANDLED;
    }
    // click, click, press (initially): light off, prep for blinkies
    else if (event == EV_click3_press) {
        set_level(0);
        return EVENT_HANDLED;
    }
    // 3 clicks: strobe mode
    else if (event == EV_3clicks) {
        set_state(strobe_beacon_state, 0);
        return EVENT_HANDLED;
    }
    #ifdef USE_BATTCHECK
    // 4 clicks: battcheck mode
    else if (event == EV_4clicks) {
        set_state(battcheck_state, 0);
        return EVENT_HANDLED;
    }
    #endif
    // hold: go to low mode, but allow ramping up
    else if (event == EV_click1_hold) {
        // don't start ramping immediately;
        // give the user time to release at low mode
        if (arg >= HOLD_TIMEOUT)
            set_state(low_mode_state, 0);
        return EVENT_HANDLED;
    }
    // hold, release quickly: go to low mode
    else if (event == EV_click1_hold_release) {
        set_state(low_mode_state, 0);
        return EVENT_HANDLED;
    }
    /* TODO: implement
    // click-release-hold: discrete ramp through all levels
    else if (event == EV_click2_hold) {
        set_state(steady_state, MAX_LEVEL);
        return EVENT_HANDLED;
    }
    */
    return EVENT_NOT_HANDLED;
}


uint8_t any_mode_state(Event event, uint16_t arg, uint8_t *primary, uint8_t *secondary, uint8_t *modes) {
    // turn on LED when entering the mode
    if (event == EV_enter_state) {
        set_any_mode(*primary, *secondary, modes);
        return EVENT_HANDLED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    // hold: change brightness (low, med, hi, always starting at low)
    else if (event == EV_click1_hold) {
        uint8_t which = arg % (HOLD_TIMEOUT * 3) / HOLD_TIMEOUT;
        switch(which) {
            case 0:
                set_state(low_mode_state, 0);
                break;
            case 1:
                set_state(med_mode_state, 0);
                break;
            case 2:
                set_state(hi_mode_state, 0);
                break;
        }
        return EVENT_HANDLED;
    }
    // 2 clicks: toggle primary/secondary level
    else if (event == EV_2clicks) {
        *primary ^= 1;
        set_any_mode(*primary, *secondary, modes);
        save_config();
        return EVENT_HANDLED;
    }
    // click-release-hold: change secondary level
    else if (event == EV_click2_hold) {
        if (arg % HOLD_TIMEOUT == 0) {
            *secondary = (*secondary + 1) & 3;
            if (! *secondary) *secondary = 1;
            *primary = 0;
            set_any_mode(*primary, *secondary, modes);
        }
        return EVENT_HANDLED;
    }
    // click, hold, release: save secondary level
    else if (event == EV_click2_hold_release) {
        save_config();
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
        return EVENT_HANDLED;
    }
    // underheating: increase slowly if we're lower than the target
    //               (proportional to how low we are)
    else if (event == EV_temperature_low) {
        if (actual_level < target_level) {
            uint8_t stepup = actual_level + (arg>>1);
            if (stepup > target_level) stepup = target_level;
            set_level(stepup);
        }
        return EVENT_HANDLED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}

uint8_t low_mode_state(Event event, uint16_t arg) {
    return any_mode_state(event, arg, &L1, &L2, low_modes);
}

uint8_t med_mode_state(Event event, uint16_t arg) {
    return any_mode_state(event, arg, &M1, &M2, med_modes);
}

uint8_t hi_mode_state(Event event, uint16_t arg) {
    return any_mode_state(event, arg, &H1, &H2, hi_modes);
}


#ifdef USE_BATTCHECK
uint8_t battcheck_state(Event event, uint16_t arg) {
    return EVENT_NOT_HANDLED;
}
#endif


uint8_t strobe_beacon_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    // 1 click (initially): cancel current blink
    // FIXME: this is no longer necessary; FSM does this automatically now
    if (event == EV_click1_release) {
        interrupt_nice_delays();
        return EVENT_HANDLED;
    }
    // 2 clicks: rotate through blinky modes
    else if (event == EV_2clicks) {
        strobe_beacon_mode = (strobe_beacon_mode + 1) & 3;
        save_config();
        interrupt_nice_delays();
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}


void low_voltage() {
    if (current_state == hi_mode_state) {
        set_state(med_mode_state, 0);
    }
    else if (current_state == med_mode_state) {
        set_state(low_mode_state, 0);
    }
    else if (current_state == low_mode_state) {
        set_state(off_state, 0);
    }
    // "step down" from blinkies to low
    else if (current_state == strobe_beacon_state) {
        set_state(low_mode_state, 0);
    }
}

void strobe(uint8_t level, uint16_t ontime, uint16_t offtime) {
    set_level(level);
    if (! nice_delay_ms(ontime)) return;
    set_level(0);
    nice_delay_ms(offtime);
}

void load_config() {
    if (load_eeprom()) {
        H1 = !(!(eeprom[0] & 0b00000100));
        M1 = !(!(eeprom[0] & 0b00000010));
        L1 = !(!(eeprom[0] & 0b00000001));
        H2 = eeprom[1];
        M2 = eeprom[2];
        L2 = eeprom[3];
        strobe_beacon_mode = eeprom[4];
    }
}

void save_config() {
    eeprom[0] = (H1<<2) | (M1<<1) | (L1);
    eeprom[1] = H2;
    eeprom[2] = M2;
    eeprom[3] = L2;
    eeprom[4] = strobe_beacon_mode;

    save_eeprom();
}

void setup() {
    set_level(RAMP_SIZE/8);
    delay_4ms(3);
    set_level(0);

    load_config();

    push_state(off_state, 0);
}

void loop() {
    if (current_state == strobe_beacon_state) {
        switch(strobe_beacon_mode) {
            // 0.2 Hz beacon at L1
            case 0:
                strobe(low_modes[0], 500, 4500);
                break;
            // 0.2 Hz beacon at H1
            case 1:
                strobe(hi_modes[0], 500, 4500);
                break;
            // 4 Hz tactical strobe at H1
            case 2:
                strobe(hi_modes[0], 83, 167);
                break;
            // 19 Hz tactical strobe at H1
            case 3:
                strobe(hi_modes[0], 17, 35);
                break;
        }
    }

    #ifdef USE_BATTCHECK
    else if (current_state == battcheck_state) {
        nice_delay_ms(500);  // wait a moment to measure voltage
        battcheck();
        set_state(off_state, 0);
    }
    #endif
}


