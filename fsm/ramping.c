// fsm-ramping.c: Ramping functions for SpaghettiMonster.
// Copyright (C) 2017-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef USE_RAMPING

#if defined(USE_AUX1_LED) || defined(USE_AUXRGB_LEDS)
#ifdef USE_AUXRGB_LEDS
#include "anduril/aux-leds.h"  // for rgb_led_voltage_readout()
#endif

void set_level_aux_leds(uint8_t level) {
    if (go_to_standby) return;

    if (! level) {
        #if defined(USE_AUX1_LED)
        set_aux1_power(0);
        #endif
        #if defined(USE_AUXRGB_LEDS)
        set_auxrgb_power(0);
        #endif
        return;
    }

    #ifdef USE_AUX1_LED
    #if defined(USE_CHANNEL_FLAGS) && (!defined(USE_AUXRGB_LEDS))
    if (channel_is_aux(channel_mode)) return;
    #endif

    #if (defined(USE_AUX1_LED_WHILE_RAMPING) || defined(USE_AUX_THRESHOLD_CONFIG))
    {
        uint8_t lvl = (cfg.aux_while_on & 0b01) ? level : 0;
        uint8_t power;
        #ifdef USE_AUX_THRESHOLD_CONFIG
        power = (lvl > cfg.aux_low_ramp_level)
             << (lvl > cfg.aux_high_ramp_level);
        #elif (USE_AUXRGB_LEDS_WHILE_ON + 0) > 0
        power = 1 + (auxrgb_level > USE_AUXRGB_LEDS_WHILE_ON);
        #else
        power = 1 + (lvl > DEFAULT_LEVEL);
        #endif
        set_aux1_power(power);
    }
    #else
    set_aux1_power(0);
    #endif  // #if (defined(USE_AUX1_LED_WHILE_RAMPING) || defined(USE_AUX_THRESHOLD_CONFIG))
    #endif  // #ifdef USE_AUX1_LED

    #ifdef USE_AUXRGB_LEDS
    #if (defined(USE_AUXRGB_LEDS_WHILE_ON) || defined(USE_AUX_THRESHOLD_CONFIG))
    // "aux while on" can't work when using aux LED channel modes
    #ifdef USE_CHANNEL_FLAGS
    if (channel_is_aux(channel_mode)) return;
    #endif

    {
        uint8_t lvl = (cfg.aux_while_on & 0b10) ? level : 0;
        #ifdef USE_SMOOTH_POVD
            // active RGB LEDs (adjustable brightness)
            uint8_t povd_level = calc_smooth_povd_brightness(lvl);
            draw_smooth_povd(povd_level);
        #else
            // passive RGB LEDs (off/low/high only)
            uint8_t power;
            #ifdef USE_AUX_THRESHOLD_CONFIG
            power = (lvl > cfg.aux_low_ramp_level)
                 << (lvl > cfg.aux_high_ramp_level);
            #elif (USE_AUXRGB_LEDS_WHILE_ON + 0) > 0
            power = 1 + (lvl > USE_AUXRGB_LEDS_WHILE_ON);
            #else
            power = 1 + (lvl > 25);
            //power = (lvl > 0) + (lvl > DEFAULT_LEVEL);
            #endif
            rgb_led_voltage_readout(power);
        #endif  // #ifdef USE_SMOOTH_POVD
    }
    #else
    set_auxrgb_power(0);
    #endif  // #if (defined(USE_AUXRGB_LEDS_WHILE_RAMPING) || defined(USE_AUX_THRESHOLD_CONFIG))
    #endif  // #ifdef USE_AUXRGB_LEDS
}
#endif  // if defined(USE_AUX1_LED) || defined(USE_AUXRGB_LEDS)


void set_level(uint8_t level) {
    #ifdef USE_RAMP_LEVEL_HARD_LIMIT
    if (ramp_level_hard_limit && (level > ramp_level_hard_limit))
        level = ramp_level_hard_limit;
    #endif

    #ifdef USE_JUMP_START
    // maybe "jump start" the engine, if it's prone to slow starts
    // (pulse the output high for a moment to wake up the power regulator)
    // (only do this when starting from off and going to a low level)
    // TODO: allow different jump start behavior per channel mode
    // FIXME: don't jump-start during factory reset
    //        (it seems to cause some eeprom issues on KR4
    //         when doing a click with a loose tailcap)
    if ((! actual_level)
            && level
            && (level < JUMP_START_LEVEL)) {
        set_level(JUMP_START_LEVEL);
        delay_4ms(JUMP_START_TIME/4);
    }
    #endif

    #if defined(USE_AUXRGB_LEDS) || defined(USE_AUX1_LED)
    set_level_aux_leds(level);
    #endif

    if (0 == level) {
        set_level_zero();
    } else {
        // call the relevant hardware-specific set_level_*()
        SetLevelFuncPtr set_level_func = channels[channel_mode].set_level;
        set_level_func(level - 1);
    }

    if (actual_level != level) prev_level = actual_level;
    actual_level = level;

    #ifdef USE_SET_LEVEL_GRADUALLY
    gradual_target = level;
    #endif

    #ifdef USE_DYNAMIC_UNDERCLOCKING
    auto_clock_speed();
    #endif
}

#ifdef USE_LEGACY_SET_LEVEL
// (this is mostly just here for reference, temporarily)
// single set of LEDs with 1 to 3 stacked power channels,
// like linear, FET+1, and FET+N+1
// (default set_level_*() function for most lights)
void set_level_legacy(uint8_t level) {
    if (level == 0) {
        #if PWM_CHANNELS >= 1
            PWM1_LVL = 0;
        #endif
        #if PWM_CHANNELS >= 2
            PWM2_LVL = 0;
        #endif
        #if PWM_CHANNELS >= 3
            PWM3_LVL = 0;
        #endif
        #if defined(PWM1_CNT) && defined(PWM1_PHASE_RESET_OFF)
            PWM1_CNT = 0;
        #endif
        #if defined(PWM2_CNT) && defined(PWM2_PHASE_RESET_OFF)
            PWM2_CNT = 0;
        #endif
        #if defined(PWM3_CNT) && defined(PWM3_PHASE_RESET_OFF)
            PWM3_CNT = 0;
        #endif
        #ifdef LED_OFF_DELAY
            // for drivers with a slow regulator chip (eg, boost converter),
            // delay before turning off to prevent flashes
            delay_4ms(LED_OFF_DELAY/4);
        #endif
        // disable the power channel, if relevant
        #ifdef LED_ENABLE_PIN
        LED_ENABLE_PORT &= ~(1 << LED_ENABLE_PIN);
        #endif
        #ifdef LED2_ENABLE_PIN
        LED2_ENABLE_PORT &= ~(1 << LED2_ENABLE_PIN);
        #endif
    } else {
        // enable the power channel, if relevant
        #ifdef LED_ENABLE_PIN
            #ifdef LED_ON_DELAY
            uint8_t led_enable_port_save = LED_ENABLE_PORT;
            #endif

            #ifndef LED_ENABLE_PIN_LEVEL_MIN
            LED_ENABLE_PORT |= (1 << LED_ENABLE_PIN);
            #else
            // only enable during part of the ramp
            if ((level >= LED_ENABLE_PIN_LEVEL_MIN)
                    && (level <= LED_ENABLE_PIN_LEVEL_MAX))
                LED_ENABLE_PORT |= (1 << LED_ENABLE_PIN);
            else  // disable during other parts of the ramp
                LED_ENABLE_PORT &= ~(1 << LED_ENABLE_PIN);
            #endif

            // for drivers with a slow regulator chip (eg, boost converter),
            // delay before lighting up to prevent flashes
            #ifdef LED_ON_DELAY
            // only delay if the pin status changed
            if (LED_ENABLE_PORT != led_enable_port_save)
                delay_4ms(LED_ON_DELAY/4);
            #endif
        #endif
        #ifdef LED2_ENABLE_PIN
            #ifdef LED2_ON_DELAY
            uint8_t led2_enable_port_save = LED2_ENABLE_PORT;
            #endif

            LED2_ENABLE_PORT |= (1 << LED2_ENABLE_PIN);

            // for drivers with a slow regulator chip (eg, boost converter),
            // delay before lighting up to prevent flashes
            #ifdef LED2_ON_DELAY
            // only delay if the pin status changed
            if (LED2_ENABLE_PORT != led2_enable_port_save)
                delay_4ms(LED2_ON_DELAY/4);
            #endif
        #endif

        // PWM array index = level - 1
        level --;

        #if PWM_CHANNELS >= 1
        PWM1_LVL = PWM_GET(pwm1_levels, level);
        #endif
        #if PWM_CHANNELS >= 2
        PWM2_LVL = PWM_GET(pwm2_levels, level);
        #endif
        #if PWM_CHANNELS >= 3
        PWM3_LVL = PWM_GET(pwm3_levels, level);
        #endif

        #ifdef USE_DYN_PWM
            uint16_t top = PWM_GET(pwm_tops, level);
            #if defined(PWM1_CNT) && defined(PWM1_PHASE_SYNC)
            // wait to ensure compare match won't be missed
            // (causes visible flickering when missed, because the counter
            //  goes all the way to 65535 before returning)
            // (see attiny1634 reference manual page 103 for a warning about
            //  the timing of changing the TOP value (section 12.8.4))
            // (but don't wait when turning on from zero, because
            //  it'll reset the phase below anyway)
            // to be safe, allow at least 32 cycles to update TOP
            while(actual_level && (PWM1_CNT > (top - 32))) {}
            #endif
            // pulse frequency modulation, a.k.a. dynamic PWM
            PWM1_TOP = top;
        #endif  // ifdef USE_DYN_PWM
        #if defined(PWM1_CNT) && defined(PWM1_PHASE_RESET_ON)
            // force reset phase when turning on from zero
            // (because otherwise the initial response is inconsistent)
            if (! actual_level) {
                PWM1_CNT = 0;
                #if defined(PWM2_CNT) && defined(PWM2_PHASE_RESET_ON)
                PWM2_CNT = 0;
                #endif
                #if defined(PWM3_CNT) && defined(PWM3_PHASE_RESET_ON)
                PWM3_CNT = 0;
                #endif
            }
        #endif
    }
    #ifdef USE_DYNAMIC_UNDERCLOCKING
    auto_clock_speed();
    #endif
}
#endif


#ifdef USE_SET_LEVEL_GRADUALLY
inline void set_level_gradually(uint8_t lvl) {
    gradual_target = lvl;
}


// call this every frame or every few frames to change brightness very smoothly
void gradual_tick() {
    uint8_t gt = gradual_target;
    if (gt < actual_level) gt = actual_level - 1;
    else if (gt > actual_level) gt = actual_level + 1;

    // call the relevant hardware-specific function
    GradualTickFuncPtr gradual_tick_func = channels[channel_mode].gradual_tick;
    bool done = gradual_tick_func(gt - 1);

    if (done) {
        uint8_t orig = gradual_target;
        set_level(gt);
        gradual_target = orig;
    }
}
#endif  // ifdef USE_SET_LEVEL_GRADUALLY


#endif  // ifdef USE_RAMPING

