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

        level --;

        #ifdef USE_TINT_RAMPING
        #ifndef TINT_RAMPING_CORRECTION
        #define TINT_RAMPING_CORRECTION 26  // 140% brightness at middle tint
        #endif
        // calculate actual PWM levels based on a single-channel ramp
        // and a global tint value
        uint8_t brightness = PWM_GET(pwm1_levels, level);
        uint8_t warm_PWM, cool_PWM;

        // auto-tint modes
        uint8_t mytint;
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

        // middle tints sag, so correct for that effect
        uint16_t base_PWM = brightness;
        // correction is only necessary when PWM is fast
        if (level > HALFSPEED_LEVEL) {
            base_PWM = brightness
                     + ((((uint16_t)brightness) * TINT_RAMPING_CORRECTION / 64) * triangle_wave(mytint) / 255);
        }

        cool_PWM = (((uint16_t)mytint * (uint16_t)base_PWM) + 127) / 255;
        warm_PWM = base_PWM - cool_PWM;

        PWM1_LVL = warm_PWM;
        PWM2_LVL = cool_PWM;
        #else  // ifdef USE_TINT_RAMPING

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

        #endif  // ifdef USE_TINT_RAMPING

        #ifdef USE_DYN_PWM
            uint16_t top = PWM_GET(pwm_tops, level);
            #ifdef PWM1_CNT
            // wait to ensure compare match won't be missed
            // (causes visible flickering when missed, because the counter
            //  goes all the way to 65535 before returning)
            // (see attiny1634 reference manual page 103 for a warning about
            //  the timing of changing the TOP value (section 12.8.4))
            // to be safe, allow at least 64 cycles to update TOP
            while(PWM1_CNT > (top - 64)) {}
            #endif
            // pulse frequency modulation, a.k.a. dynamic PWM
            PWM1_TOP = top;

            // repeat for other channels if necessary
            #ifdef PMW2_TOP
                #ifdef PWM2_CNT
                while(PWM2_CNT > (top - 64)) {}
                #endif
                PWM2_TOP = top;
            #endif
            #ifdef PMW3_TOP
                #ifdef PWM3_CNT
                while(PWM3_CNT > (top - 64)) {}
                #endif
                PWM3_TOP = top;
            #endif
        #endif  // ifdef USE_DYN_PWM
    }
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

    #ifdef LED_ENABLE_PIN_LEVEL_MIN
    // only enable during part of the ramp
    if ((gt >= LED_ENABLE_PIN_LEVEL_MIN)
            && (gt <= LED_ENABLE_PIN_LEVEL_MAX))
        LED_ENABLE_PORT |= (1 << LED_ENABLE_PIN);
    else  // disable during other parts of the ramp
        LED_ENABLE_PORT &= ~(1 << LED_ENABLE_PIN);
    #endif

    gt --;  // convert 1-based number to 0-based

    PWM_DATATYPE target;

    #if PWM_CHANNELS >= 1
    target = PWM_GET(pwm1_levels, gt);
    if ((gt < actual_level)     // special case for FET-only turbo
            && (PWM1_LVL == 0)  // (bypass adjustment period for first step)
            && (target == PWM_TOP)) PWM1_LVL = PWM_TOP;
    else if (PWM1_LVL < target) PWM1_LVL ++;
    else if (PWM1_LVL > target) PWM1_LVL --;
    #endif
    #if PWM_CHANNELS >= 2
    target = PWM_GET(pwm2_levels, gt);
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
        actual_level = gt + 1;
    }
    #ifdef USE_DYNAMIC_UNDERCLOCKING
    auto_clock_speed();
    #endif
}
#endif  // ifdef OVERRIDE_GRADUAL_TICK
#endif  // ifdef USE_SET_LEVEL_GRADUALLY

#endif  // ifdef USE_RAMPING
#endif
