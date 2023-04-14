/*
 * fsm-ramping.c: Ramping functions for SpaghettiMonster.
 * Handles 1- to 4-channel smooth ramping on a single LED.
 *
 * Copyright (C) 2017-2023 Selene ToyKeeper
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#ifdef USE_RAMPING

void set_channel_mode(uint8_t mode) {
    uint8_t cur_level = actual_level;
    // turn off old LEDs before changing channel
    set_level(0);

    // change the channel
    channel_mode = mode;

    // update the LEDs
    set_level(cur_level);
}

#ifdef HAS_AUX_LEDS
inline void set_level_aux_leds(uint8_t level) {
    #ifdef USE_INDICATOR_LED_WHILE_RAMPING
        // use side-facing aux LEDs while main LEDs are on
        if (! go_to_standby) {
        #ifdef USE_INDICATOR_LED
            indicator_led((level > 0) + (level > DEFAULT_LEVEL));
        #endif
        #ifdef USE_BUTTON_LED
            button_led_set((level > 0) + (level > DEFAULT_LEVEL));
        #endif
        }
    #else  // turn off front-facing aux LEDs while main LEDs are on
        #if defined(USE_INDICATOR_LED) || defined(USE_AUX_RGB_LEDS)
        if (! go_to_standby) {
            #ifdef USE_INDICATOR_LED
                indicator_led(0);
            #endif
            #ifdef USE_AUX_RGB_LEDS
                rgb_led_set(0);
                #ifdef USE_BUTTON_LED
                    button_led_set((level > 0) + (level > DEFAULT_LEVEL));
                #endif
            #endif
        }
        #endif
    #endif
}
#endif  // ifdef HAS_AUX_LEDS


void set_level(uint8_t level) {
    #ifdef USE_JUMP_START
    // maybe "jump start" the engine, if it's prone to slow starts
    // (pulse the output high for a moment to wake up the power regulator)
    // (only do this when starting from off and going to a low level)
    // TODO: allow different jump start behavior per channel mode
    if ((! actual_level)
            && level
            && (level < jump_start_level)) {
        set_level(jump_start_level);
        delay_4ms(JUMP_START_TIME/4);
    }
    #endif

    #ifdef HAS_AUX_LEDS
    set_level_aux_leds(level);
    #endif

    // call the relevant hardware-specific set_level_*()
    SetLevelFuncPtr set_level_func = channel_modes[channel_mode];
    set_level_func(level);

    actual_level = level;

    #ifdef USE_SET_LEVEL_GRADUALLY
    gradual_target = level;
    #endif

    #ifdef USE_DYNAMIC_UNDERCLOCKING
    auto_clock_speed();
    #endif
}

///// Common set_level_*() functions shared by multiple lights /////
// (unique lights should use their own,
//  but these common versions cover most of the common hardware designs)

#ifdef USE_SET_LEVEL_1CH
// single set of LEDs with 1 power channel
void set_level_1ch(uint8_t level) {
    if (level == 0) {
        LOW_PWM_LVL  = 0;
    } else {
        level --;  // PWM array index = level - 1
        LOW_PWM_LVL  = PWM_GET(low_pwm_levels, level);
    }
}
#endif

#ifdef USE_SET_LEVEL_2CH_STACKED
// single set of LEDs with 2 stacked power channels, DDFET+1 or DDFET+linear
void set_level_2ch_stacked(uint8_t level) {
    if (level == 0) {
        LOW_PWM_LVL  = 0;
        HIGH_PWM_LVL = 0;
    } else {
        level --;  // PWM array index = level - 1
        LOW_PWM_LVL  = PWM_GET(low_pwm_levels,  level);
        HIGH_PWM_LVL = PWM_GET(high_pwm_levels, level);
    }
}
#endif

#ifdef USE_SET_LEVEL_3CH_STACKED
// single set of LEDs with 3 stacked power channels, like DDFET+N+1
void set_level_3ch_stacked(uint8_t level) {
    if (level == 0) {
        LOW_PWM_LVL  = 0;
        MED_PWM_LVL  = 0;
        HIGH_PWM_LVL = 0;
    } else {
        level --;  // PWM array index = level - 1
        LOW_PWM_LVL  = PWM_GET(low_pwm_levels,  level);
        MED_PWM_LVL  = PWM_GET(med_pwm_levels,  level);
        HIGH_PWM_LVL = PWM_GET(high_pwm_levels, level);
    }
}
#endif

// TODO: upgrade some older lights to dynamic PWM
// TODO: 1ch w/ dynamic PWM
// TODO: 1ch w/ dynamic PWM and opamp enable pins?
// TODO: 2ch stacked w/ dynamic PWM
// TODO: 2ch stacked w/ dynamic PWM and opamp enable pins?

#ifdef USE_SET_LEVEL_2CH_BLEND
// warm + cool blend w/ middle sag correction
void set_level_2ch_blend(uint8_t level) {
    #ifndef TINT_RAMPING_CORRECTION
    #define TINT_RAMPING_CORRECTION 26  // 140% brightness at middle tint
    #endif

    BLEND_PWM_DATATYPE vpwm;

    if (level == 0) {
        vpwm  = 0;
    } else {
        level --;  // PWM array index = level - 1
        vpwm  = PWM_GET(blend_pwm_levels, level);
    }

    // calculate actual PWM levels based on a single-channel ramp
    // and a global tint value
    uint16_t brightness = vpwm;
    uint16_t warm_PWM, cool_PWM;
    const uint16_t top = PWM_TOP;

    // auto-tint modes
    uint8_t mytint = channel_mode_args[channel_mode];

    PWM_DATATYPE2 base_PWM = brightness;
    #if defined(TINT_RAMPING_CORRECTION) && (TINT_RAMPING_CORRECTION > 0)
        // middle tints sag, so correct for that effect
        // by adding extra power which peaks at the middle tint
        // (correction is only necessary when PWM is fast)
        if (level > HALFSPEED_LEVEL) {
            base_PWM = brightness
                     + ((((PWM_DATATYPE2)brightness) * TINT_RAMPING_CORRECTION / 64)
                        * triangle_wave(mytint) / 255);
        }
        // fade the triangle wave out when above 100% power,
        // so it won't go over 200%
        if (brightness > top) {
            base_PWM -= 2 * (
                             ((brightness - top) * TINT_RAMPING_CORRECTION / 64)
                             * triangle_wave(mytint) / 255
                        );
        }
        // guarantee no more than 200% power
        if (base_PWM > (top << 1)) { base_PWM = top << 1; }
    #endif

    cool_PWM = (((PWM_DATATYPE2)mytint * (PWM_DATATYPE2)base_PWM) + 127) / 255;
    warm_PWM = base_PWM - cool_PWM;
    // when running at > 100% power, spill extra over to other channel
    if (cool_PWM > top) {
        warm_PWM += (cool_PWM - top);
        cool_PWM = top;
    } else if (warm_PWM > top) {
        cool_PWM += (warm_PWM - top);
        warm_PWM = top;
    }

    WARM_PWM_LVL = warm_PWM;
    COOL_PWM_LVL = cool_PWM;
}
#endif  // ifdef USE_TINT_RAMPING

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
    // call the relevant hardware-specific function
    GradualTickFuncPtr gradual_tick_func = gradual_tick_modes[channel_mode];
    gradual_tick_func();
}


// reduce repetition with macros
// common code at the beginning of every gradual tick handler
#define GRADUAL_TICK_SETUP()  \
    uint8_t gt = gradual_target;  \
    if (gt < actual_level) gt = actual_level - 1;  \
    else if (gt > actual_level) gt = actual_level + 1;  \
    gt --;  \
    PWM_DATATYPE target;

// tick the top layer of the stack
#define GRADUAL_ADJUST_1CH(TABLE,PWM)  \
    target = PWM_GET(TABLE, gt);  \
    if (PWM < target) PWM ++;  \
    else if (PWM > target) PWM --;

// tick a base level of the stack
// (with support for special DD FET behavior
//  like "low=0, high=255" --> "low=255, high=254")
#define GRADUAL_ADJUST(TABLE,PWM,TOP)  \
    target = PWM_GET(TABLE, gt);  \
    if ((gt < actual_level)  \
        && (PWM == 0)  \
        && (target == TOP)) PWM = TOP;  \
    else  \
    if (PWM < target) PWM ++;  \
    else if (PWM > target) PWM --;

// do this when output exactly matches a ramp level
#define GRADUAL_IS_ACTUAL()  \
    uint8_t orig = gradual_target;  \
    set_level(gt + 1);  \
    gradual_target = orig;

#ifdef USE_GRADUAL_TICK_1CH
void gradual_tick_1ch() {
    GRADUAL_TICK_SETUP();

    GRADUAL_ADJUST_1CH(low_pwm_levels, LOW_PWM_LVL);

    // did we go far enough to hit the next defined ramp level?
    // if so, update the main ramp level tracking var
    if ((LOW_PWM_LVL  == PWM_GET(low_pwm_levels,  gt)))
    {
        GRADUAL_IS_ACTUAL();
    }
}
#endif

#ifdef USE_GRADUAL_TICK_2CH_STACKED
void gradual_tick_2ch_stacked() {
    GRADUAL_TICK_SETUP();

    GRADUAL_ADJUST(low_pwm_levels, LOW_PWM_LVL, PWM_TOP);
    GRADUAL_ADJUST_1CH(high_pwm_levels, HIGH_PWM_LVL);

    // did we go far enough to hit the next defined ramp level?
    // if so, update the main ramp level tracking var
    if (   (LOW_PWM_LVL  == PWM_GET(low_pwm_levels,  gt))
        && (HIGH_PWM_LVL == PWM_GET(high_pwm_levels, gt))
       )
    {
        GRADUAL_IS_ACTUAL();
    }
}
#endif

#ifdef USE_GRADUAL_TICK_3CH_STACKED
void gradual_tick_3ch_stacked() {
    GRADUAL_TICK_SETUP();

    GRADUAL_ADJUST(low_pwm_levels, LOW_PWM_LVL, PWM_TOP);
    GRADUAL_ADJUST(med_pwm_levels, MED_PWM_LVL, PWM_TOP);
    GRADUAL_ADJUST_1CH(high_pwm_levels, HIGH_PWM_LVL);

    // did we go far enough to hit the next defined ramp level?
    // if so, update the main ramp level tracking var
    if (   (LOW_PWM_LVL  == PWM_GET(low_pwm_levels,  gt))
        && (MED_PWM_LVL  == PWM_GET(med_pwm_levels,  gt))
        && (HIGH_PWM_LVL == PWM_GET(high_pwm_levels, gt))
        )
    {
        GRADUAL_IS_ACTUAL();
    }
}
#endif
#endif  // ifdef USE_SET_LEVEL_GRADUALLY


#if defined(USE_TINT_RAMPING) && (!defined(TINT_RAMP_TOGGLE_ONLY))
void set_level_2ch_blend() {
    #ifndef TINT_RAMPING_CORRECTION
    #define TINT_RAMPING_CORRECTION 26  // 140% brightness at middle tint
    #endif

    // calculate actual PWM levels based on a single-channel ramp
    // and a global tint value
    //PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    uint16_t brightness = PWM1_LVL;
    uint16_t warm_PWM, cool_PWM;
    #ifdef USE_STACKED_DYN_PWM
        uint16_t top = PWM1_TOP;
        //uint16_t top = PWM_GET(pwm_tops, actual_level-1);
    #else
        const uint16_t top = PWM_TOP;
    #endif

    // auto-tint modes
    uint8_t mytint;
    uint8_t level = actual_level - 1;
    #if 1
    // perceptual by ramp level
    if (tint == 0) { mytint = 255 * (uint16_t)level / RAMP_SIZE; }
    else if (tint == 255) { mytint = 255 - (255 * (uint16_t)level / RAMP_SIZE); }
    #else
    // linear with power level
    //if (tint == 0) { mytint = brightness; }
    //else if (tint == 255) { mytint = 255 - brightness; }
    #endif
    // stretch 1-254 to fit 0-255 range (hits every value except 98 and 198)
    else { mytint = (tint * 100 / 99) - 1; }

    PWM_DATATYPE2 base_PWM = brightness;
    #if defined(TINT_RAMPING_CORRECTION) && (TINT_RAMPING_CORRECTION > 0)
        // middle tints sag, so correct for that effect
        // by adding extra power which peaks at the middle tint
        // (correction is only necessary when PWM is fast)
        if (level > HALFSPEED_LEVEL) {
            base_PWM = brightness
                     + ((((PWM_DATATYPE2)brightness) * TINT_RAMPING_CORRECTION / 64) * triangle_wave(mytint) / 255);
        }
        // fade the triangle wave out when above 100% power,
        // so it won't go over 200%
        if (brightness > top) {
            base_PWM -= 2 * (
                             ((brightness - top) * TINT_RAMPING_CORRECTION / 64)
                             * triangle_wave(mytint) / 255
                        );
        }
        // guarantee no more than 200% power
        if (base_PWM > (top << 1)) { base_PWM = top << 1; }
    #endif

    cool_PWM = (((PWM_DATATYPE2)mytint * (PWM_DATATYPE2)base_PWM) + 127) / 255;
    warm_PWM = base_PWM - cool_PWM;
    // when running at > 100% power, spill extra over to other channel
    if (cool_PWM > top) {
        warm_PWM += (cool_PWM - top);
        cool_PWM = top;
    } else if (warm_PWM > top) {
        cool_PWM += (warm_PWM - top);
        warm_PWM = top;
    }

    TINT1_LVL = warm_PWM;
    TINT2_LVL = cool_PWM;

    // disable the power channel, if relevant
    #ifdef LED_ENABLE_PIN
    if (warm_PWM)
        LED_ENABLE_PORT |= (1 << LED_ENABLE_PIN);
    else
        LED_ENABLE_PORT &= ~(1 << LED_ENABLE_PIN);
    #endif
    #ifdef LED2_ENABLE_PIN
    if (cool_PWM)
        LED2_ENABLE_PORT |= (1 << LED2_ENABLE_PIN);
    else
        LED2_ENABLE_PORT &= ~(1 << LED2_ENABLE_PIN);
    #endif
}
#endif  // ifdef USE_TINT_RAMPING


// define the channel mode lists
// TODO: move to progmem
SetLevelFuncPtr channel_modes[NUM_CHANNEL_MODES] = { SET_LEVEL_MODES };
#ifdef USE_SET_LEVEL_GRADUALLY
GradualTickFuncPtr gradual_tick_modes[NUM_CHANNEL_MODES] = { GRADUAL_TICK_MODES };
#endif


#endif  // ifdef USE_RAMPING
