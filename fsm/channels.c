// fsm-channels.c: Channel mode functions for SpaghettiMonster.
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "fsm/ramping.h"


#if NUM_CHANNEL_MODES > 1
void set_channel_mode(uint8_t mode) {
    if (mode == channel_mode) return;  // abort if nothing to do

    uint8_t cur_level = actual_level;

    // turn off old LEDs before changing channel
    set_level(0);

    // change the channel
    channel_mode = mode;

    // update the LEDs
    set_level(cur_level);
}
#endif  // if NUM_CHANNEL_MODES > 1


#ifdef USE_CALC_2CH_BLEND
// calculate a "tint ramp" blend between 2 channels
// results are placed in *warm and *cool vars
// brightness : total amount of light units to distribute
// top : maximum allowed brightness per channel
// blend : ratio between warm and cool (0 = warm, 128 = 50%, 255 = cool)
void calc_2ch_blend(
    PWM_DATATYPE *warm,
    PWM_DATATYPE *cool,
    PWM_DATATYPE brightness,
    PWM_DATATYPE top,
    uint8_t blend) {

    #ifndef TINT_RAMPING_CORRECTION
    #define TINT_RAMPING_CORRECTION 26  // 140% brightness at middle tint
    #endif

    // calculate actual PWM levels based on a single-channel ramp
    // and a blend value
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE2 base_PWM = brightness;

    #if defined(TINT_RAMPING_CORRECTION) && (TINT_RAMPING_CORRECTION > 0)
        uint8_t level = actual_level - 1;

        // middle tints sag, so correct for that effect
        // by adding extra power which peaks at the middle tint
        // (correction is only necessary when PWM is fast)
        if (level > HALFSPEED_LEVEL) {
            base_PWM = brightness
                     + ((((PWM_DATATYPE2)brightness) * TINT_RAMPING_CORRECTION / 64)
                        * triangle_wave(blend) / 255);
        }
        // fade the triangle wave out when above 100% power,
        // so it won't go over 200%
        if (brightness > top) {
            base_PWM -= 2 * (
                             ((brightness - top) * TINT_RAMPING_CORRECTION / 64)
                             * triangle_wave(blend) / 255
                        );
        }
        // guarantee no more than 200% power
        if (base_PWM > (top << 1)) { base_PWM = top << 1; }
    #endif

    cool_PWM = (((PWM_DATATYPE2)blend * (PWM_DATATYPE2)base_PWM) + 127) / 255;
    warm_PWM = base_PWM - cool_PWM;
    // when running at > 100% power, spill extra over to other channel
    if (cool_PWM > top) {
        warm_PWM += (cool_PWM - top);
        cool_PWM = top;
    } else if (warm_PWM > top) {
        cool_PWM += (warm_PWM - top);
        warm_PWM = top;
    }

    *warm = warm_PWM;
    *cool = cool_PWM;
}
#endif  // ifdef USE_CALC_2CH_BLEND


#ifdef USE_HSV2RGB
RGB_t hsv2rgb(uint8_t h, uint8_t s, uint16_t v) {
    RGB_t color;

    if (s == 0) {  // grey
        color.r = color.g = color.b = v;
        return color;
    }

    uint8_t region;
    uint16_t fpart;
    uint16_t high, low, rising, falling;

    // hue has 6 segments, 0-5
    region = ((uint16_t)h * 6) >> 8;
    // find remainder part, make it from 0-255
    fpart = ((uint16_t)h * 6) - (region << 8);

    // calculate graph segments, doing integer multiplication
    // TODO: calculate 16-bit results, not 8-bit
    high    = v;
    low     = ((uint32_t)v * (255 - s)) >> 8;
    // TODO: use a cosine crossfade instead of linear
    // (because it looks better and feels more natural)
    falling = ((uint32_t)v * (255 - ((s * fpart) >> 8))) >> 8;
    rising  = ((uint32_t)v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;

    // default floor
    color.r = low;
    color.g = low;
    color.b = low;

    // assign graph shapes based on color cone region
    switch (region) {
        case 0:
            color.r = high;
            color.g = rising;
            //color.b = low;
            break;
        case 1:
            color.r = falling;
            color.g = high;
            //color.b = low;
            break;
        case 2:
            //color.r = low;
            color.g = high;
            color.b = rising;
            break;
        case 3:
            //color.r = low;
            color.g = falling;
            color.b = high;
            break;
        case 4:
            color.r = rising;
            //color.g = low;
            color.b = high;
            break;
        default:
            color.r = high;
            //color.g = low;
            color.b = falling;
            break;
    }

    return color;
}
#endif  // ifdef USE_HSV2RGB


///// Common set_level_*() functions shared by multiple lights /////
// (unique lights should use their own,
//  but these common versions cover most of the common hardware designs)

// TODO: upgrade some older lights to dynamic PWM
// TODO: 1ch w/ dynamic PWM
// TODO: 1ch w/ dynamic PWM and opamp enable pins?
// TODO: 2ch stacked w/ dynamic PWM
// TODO: 2ch stacked w/ dynamic PWM and opamp enable pins?


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


