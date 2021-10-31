/*
 * fsm-ramping.c: Ramping functions for SpaghettiMonster.
 * Handles 1- to 4-channel smooth ramping on a single LED.
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

#ifndef FSM_RAMPING_C
#define FSM_RAMPING_C

#ifdef USE_RAMPING

void set_level(uint8_t level) {
    #ifdef USE_JUMP_START
    // maybe "jump start" the engine, if it's prone to slow starts
    // (pulse the output high for a moment to wake up the power regulator)
    // (only do this when starting from off and going to a low level)
    if ((! actual_level)
            && level
            && (level < jump_start_level)) {
        set_level(jump_start_level);
        delay_4ms(JUMP_START_TIME/4);
    }
    #endif  // ifdef USE_JUMP_START

    actual_level = level;

    #ifdef USE_SET_LEVEL_GRADUALLY
    gradual_target = level;
    #endif

    #ifdef USE_INDICATOR_LED_WHILE_RAMPING
        #ifdef USE_INDICATOR_LED
        if (! go_to_standby)
            indicator_led((level > 0) + (level > DEFAULT_LEVEL));
        #endif
        //if (level > MAX_1x7135) indicator_led(2);
        //else if (level > 0) indicator_led(1);
        //else if (! go_to_standby) indicator_led(0);
    #else
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

    #ifdef OVERRIDE_SET_LEVEL
        set_level_override(level);
    #else

    #ifdef PWM1_CNT
    static uint8_t prev_level = 0;
    uint8_t api_level = level;
    #endif

    //TCCR0A = PHASE;
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
        #if PWM_CHANNELS >= 4
        PWM4_LVL = 0;
        #endif
        #ifdef USE_TINT_RAMPING
        TINT1_LVL = 0;
        TINT2_LVL = 0;
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
        #ifndef USE_TINT_RAMPING  // update_tint handles this better
        #ifdef LED_ENABLE_PIN
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
        #endif
        #ifdef LED2_ENABLE_PIN
        LED2_ENABLE_PORT |= (1 << LED2_ENABLE_PIN);
        #endif
        #endif  // ifndef USE_TINT_RAMPING

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
        #if PWM_CHANNELS >= 4
        PWM4_LVL = PWM_GET(pwm4_levels, level);
        #endif

        #ifdef USE_DYN_PWM
            uint16_t top = PWM_GET(pwm_tops, level);
            #ifdef PWM1_CNT
            // wait to ensure compare match won't be missed
            // (causes visible flickering when missed, because the counter
            //  goes all the way to 65535 before returning)
            // (see attiny1634 reference manual page 103 for a warning about
            //  the timing of changing the TOP value (section 12.8.4))
            // (but don't wait when turning on from zero, because
            //  it'll reset the phase below anyway)
            // to be safe, allow at least 64 cycles to update TOP
            while(prev_level && (PWM1_CNT > (top - 64))) {}
            #endif
            // pulse frequency modulation, a.k.a. dynamic PWM
            PWM1_TOP = top;

            // repeat for other channels if necessary
            #ifdef PMW2_TOP
                #ifdef PWM2_CNT
                while(prev_level && (PWM2_CNT > (top - 64))) {}
                #endif
                PWM2_TOP = top;
            #endif
            #ifdef PMW3_TOP
                #ifdef PWM3_CNT
                while(prev_level && (PWM3_CNT > (top - 64))) {}
                #endif
                PWM3_TOP = top;
            #endif
        #endif  // ifdef USE_DYN_PWM
        #ifdef PWM1_CNT
            // force reset phase when turning on from zero
            // (because otherwise the initial response is inconsistent)
            if (! prev_level) {
                PWM1_CNT = 0;
                #ifdef PWM2_CNT
                PWM2_CNT = 0;
                #endif
                #ifdef PWM3_CNT
                PWM3_CNT = 0;
                #endif
            }
        #endif
    }
    #ifdef USE_TINT_RAMPING
    update_tint();
    #endif

    #ifdef PWM1_CNT
    prev_level = api_level;
    #endif
    #endif  // ifdef OVERRIDE_SET_LEVEL
    #ifdef USE_DYNAMIC_UNDERCLOCKING
    auto_clock_speed();
    #endif
}

#ifdef USE_SET_LEVEL_GRADUALLY
inline void set_level_gradually(uint8_t lvl) {
    gradual_target = lvl;
}

#ifndef OVERRIDE_GRADUAL_TICK
// call this every frame or every few frames to change brightness very smoothly
void gradual_tick() {
    // go by only one ramp level at a time instead of directly to the target
    uint8_t gt = gradual_target;
    if (gt < actual_level) gt = actual_level - 1;
    else if (gt > actual_level) gt = actual_level + 1;

    /*
    #ifdef LED_ENABLE_PIN_LEVEL_MIN
    // only enable during part of the ramp
    if ((gt >= LED_ENABLE_PIN_LEVEL_MIN)
            && (gt <= LED_ENABLE_PIN_LEVEL_MAX))
        LED_ENABLE_PORT |= (1 << LED_ENABLE_PIN);
    else  // disable during other parts of the ramp
        LED_ENABLE_PORT &= ~(1 << LED_ENABLE_PIN);
    #endif
    */

    gt --;  // convert 1-based number to 0-based

    PWM_DATATYPE target;

    #if PWM_CHANNELS >= 1
    target = PWM_GET(pwm1_levels, gt);
        #if PWM_CHANNELS > 1
        if ((gt < actual_level)     // special case for FET-only turbo
                && (PWM1_LVL == 0)  // (bypass adjustment period for first step)
                && (target == PWM_TOP)) PWM1_LVL = PWM_TOP;
        else
        #endif
    if (PWM1_LVL < target) PWM1_LVL ++;
    else if (PWM1_LVL > target) PWM1_LVL --;
    #endif
    #if PWM_CHANNELS >= 2
    target = PWM_GET(pwm2_levels, gt);
        #if PWM_CHANNELS > 2
        if ((gt < actual_level)     // special case for FET-only turbo
                && (PWM2_LVL == 0)  // (bypass adjustment period for first step)
                && (target == PWM_TOP)) PWM2_LVL = PWM_TOP;
        else
        #endif
    if (PWM2_LVL < target) PWM2_LVL ++;
    else if (PWM2_LVL > target) PWM2_LVL --;
    #endif
    #if PWM_CHANNELS >= 3
    target = PWM_GET(pwm3_levels, gt);
    if (PWM3_LVL < target) PWM3_LVL ++;
    else if (PWM3_LVL > target) PWM3_LVL --;
    #endif
    #if PWM_CHANNELS >= 4
    target = PWM_GET(pwm4_levels, gt);
    if (PWM4_LVL < target) PWM4_LVL ++;
    else if (PWM4_LVL > target) PWM4_LVL --;
    #endif

    // did we go far enough to hit the next defined ramp level?
    // if so, update the main ramp level tracking var
    if ((PWM1_LVL == PWM_GET(pwm1_levels, gt))
        #if PWM_CHANNELS >= 2
            && (PWM2_LVL == PWM_GET(pwm2_levels, gt))
        #endif
        #if PWM_CHANNELS >= 3
            && (PWM3_LVL == PWM_GET(pwm3_levels, gt))
        #endif
        #if PWM_CHANNELS >= 4
            && (PWM4_LVL == PWM_GET(pwm4_levels, gt))
        #endif
        )
    {
        //actual_level = gt + 1;
        uint8_t orig = gradual_target;
        set_level(gt + 1);
        gradual_target = orig;
    }
    // is handled in set_level()
    //#ifdef USE_TINT_RAMPING
    //update_tint();
    //#endif
    // is handled in set_level()
    //#ifdef USE_DYNAMIC_UNDERCLOCKING
    //auto_clock_speed();
    //#endif
}
#endif  // ifdef OVERRIDE_GRADUAL_TICK
#endif  // ifdef USE_SET_LEVEL_GRADUALLY


#if defined(USE_TINT_RAMPING) && (!defined(TINT_RAMP_TOGGLE_ONLY))
void update_tint() {
    #ifndef TINT_RAMPING_CORRECTION
    #define TINT_RAMPING_CORRECTION 26  // 140% brightness at middle tint
    #endif

    // calculate actual PWM levels based on a single-channel ramp
    // and a global tint value
    //PWM_DATATYPE brightness = PWM_GET(pwm1_levels, level);
    uint16_t brightness = PWM1_LVL;
    uint16_t warm_PWM, cool_PWM;
    #ifdef USE_DYN_PWM
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


#endif  // ifdef USE_RAMPING
#endif
