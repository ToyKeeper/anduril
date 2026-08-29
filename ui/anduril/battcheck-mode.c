// battcheck-mode.c: Battery check mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "anduril/battcheck-mode.h"

uint8_t battcheck_state(Event event, uint16_t arg) {
    #ifdef USE_SMOOTH_POVD
    static uint8_t lvp_counter = 0;
    #endif  // #ifdef USE_SMOOTH_POVD

    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }

    #ifdef USE_SMOOTH_POVD
    // 1H: toggle Batt Color mode
    else if (event == EV_click1_hold) {
        if (! arg) {
            batt_color_mode = (! batt_color_mode);
            blink_once();
        }
        return EVENT_HANDLED;
    }
    #endif  // #ifdef USE_SMOOTH_POVD

    #ifdef USE_POWERBANK_HOST_MODE
    // 2H: toggle Powerbank Host mode
    else if (event == EV_click2_hold) {
        if (! arg) {
            if (POWER_BANK_EN_PORT & POWER_BANK_EN_PIN)
                POWER_BANK_EN_PORT &= (~POWER_BANK_EN_PIN);
            else
                POWER_BANK_EN_PORT |= POWER_BANK_EN_PIN;
            blink_once();
        }
        return EVENT_HANDLED;
    }
    // turn off powerbank host mode when exiting Batt Check
    else if (event == EV_leave_state) {
        POWER_BANK_EN_PORT &= (~POWER_BANK_EN_PIN);
    }
    #endif  // ifdef USE_POWERBANK_HOST_MODE

    #ifdef USE_SIMPLE_UI
    else if (cfg.simple_ui_active) {
        // any other button event is completed: exit
        //if ((B_CLICK|B_TIMEOUT) == (event & (B_CLICK|B_TIMEOUT))) {
        //    set_state(off_state, 0);
        //}
        return EVENT_NOT_HANDLED;
    }
    #endif

    ////////// Every action below here is blocked in the simple UI //////////

    // 2 clicks: next blinky mode
    else if (event == EV_2clicks) {
        #if defined(USE_THERMAL_REGULATION)
        set_state(tempcheck_state, 0);
        #elif defined(USE_BEACON_MODE)
        set_state(beacon_state, 0);
        #elif defined(USE_SOS_MODE) && defined(USE_SOS_MODE_IN_BLINKY_GROUP)
        set_state(sos_state, 0);
        #endif
        return EVENT_HANDLED;
    }

    #ifdef DEFAULT_BLINK_CHANNEL
    // 3 clicks: next channel mode (specific to number blinky modes)
    else if (event == EV_3clicks) {
        cfg.blink_channel = (cfg.blink_channel + 1) % NUM_CHANNEL_MODES;
        save_config();
        return EVENT_HANDLED;
    }
    #endif  // ifdef DEFAULT_BLINK_CHANNEL

    #ifdef USE_VOLTAGE_CORRECTION
    // 7H: voltage config mode
    else if (event == EV_click7_hold) {
        push_state(voltage_config_state, 0);
        return EVENT_HANDLED;
    }
    #endif

    #ifdef USE_SMOOTH_POVD
    // FIXME? use a better symbol for this, like USE_BATT_COLOR_MODE or similar
    else if (event == EV_voltage_low) {
        // while charging, chargers can sometimes drop voltage really low for a moment,
        // so don't react unless it stays low for a while
        lvp_counter ++;
        if (lvp_counter > 16) return EVENT_NOT_HANDLED;  // allow light to turn off
        else return EVENT_HANDLED;  // ignore until we're sure
    }
    else if (! voltage_is_low) {
        lvp_counter = 0;
    }
    #endif  // #ifdef USE_SMOOTH_POVD

    return EVENT_NOT_HANDLED;
}

void battcheck_iter() {
    #ifdef USE_SMOOTH_POVD
    if (batt_color_mode) {
        // force ADC into voltage mode, and update 'voltage' var
        if (adc_channel) {  // force voltage, not temperature
            adc_voltage_mode();
        }
        else {
            // update cooked voltage measurement
            v16_force_update();
            // draw this frame
            draw_smooth_povd(RAMP_SIZE);
            // wait until next frame
            nice_delay_ms(16);
        }
        return;
    }
    #endif  // #ifdef USE_SMOOTH_POVD

    nice_delay_ms(1000);  // wait a moment for a more accurate reading
    battcheck();
    #ifdef USE_SIMPLE_UI
    // in simple mode, turn off after one readout
    // FIXME: can eat the next button press
    //        (state changes in loop() act weird)
    if (cfg.simple_ui_active) set_state_deferred(off_state, 0);
    else nice_delay_ms(1000);
    #endif
}

#if defined(USE_VOLTAGE_CORRECTION) || defined(USE_POST_OFF_VOLTAGE) || defined(USE_AUX_THRESHOLD_CONFIG)
// the user can adjust the battery measurements... on a scale of 1 to 13
// 1 = subtract 0.30V
// 2 = subtract 0.25V
// ...
// 7 = no effect (add 0V)
// 8 = add 0.05V
// ...
// 13 = add 0.30V
void voltage_config_save(uint8_t step, uint8_t value) {
    switch (step) {
        #if defined(USE_AUX1_LED) || defined(USE_AUXRGB_LEDS)
        case aux_while_on_step:
            cfg.aux_while_on = value;
            break;
        #endif
        #if defined(USE_AUX_THRESHOLD_CONFIG)
        case aux_low_ramp_level_step:
            // 0 clicks = 255 = never turn on
            cfg.aux_low_ramp_level = value - 1;
            break;
        case aux_high_ramp_level_step:
            // 0 clicks = 255 = never turn on
            cfg.aux_high_ramp_level = value - 1;
            break;
        #endif

        #ifdef USE_POST_OFF_VOLTAGE
        case post_off_voltage_config_step:
            cfg.post_off_voltage = value;
            break;
        #endif

        #ifdef USE_VOLTAGE_CORRECTION
        default:
            if (value) cfg.voltage_correction = value;
            break;
        #endif
    }
}

uint8_t voltage_config_state(Event event, uint16_t arg) {
    return config_state_base(event, arg,
                             voltage_config_num_steps - 1,
                             voltage_config_save);
}
#endif  // #ifdef USE_VOLTAGE_CORRECTION

