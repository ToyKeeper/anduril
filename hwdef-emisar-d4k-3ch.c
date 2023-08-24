// Emisar D4K 3-channel hwdef
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "chan-rgbaux.c"


void set_level_main2(uint8_t level);
void set_level_led3(uint8_t level);
void set_level_led4(uint8_t level);
void set_level_all(uint8_t level);
void set_level_led34a_blend(uint8_t level);
void set_level_led34b_blend(uint8_t level);
void set_level_hsv(uint8_t level);
void set_level_auto3(uint8_t level);

bool gradual_tick_main2(uint8_t gt);
bool gradual_tick_led3(uint8_t gt);
bool gradual_tick_led4(uint8_t gt);
bool gradual_tick_all(uint8_t gt);
bool gradual_tick_led34a_blend(uint8_t gt);
bool gradual_tick_led34b_blend(uint8_t gt);
bool gradual_tick_hsv(uint8_t gt);
bool gradual_tick_auto3(uint8_t gt);


Channel channels[] = {
    { // main 2 LEDs only
        .set_level    = set_level_main2,
        .gradual_tick = gradual_tick_main2,
        .has_args     = 0
    },
    { // 3rd LED only
        .set_level    = set_level_led3,
        .gradual_tick = gradual_tick_led3,
        .has_args     = 0
    },
    { // 4th LED only
        .set_level    = set_level_led4,
        .gradual_tick = gradual_tick_led4,
        .has_args     = 0
    },
    { // all channels, tied together (equal amounts, max power)
        .set_level    = set_level_all,
        .gradual_tick = gradual_tick_all,
        .has_args     = 0
    },
    { // 3rd + 4th LEDs, manual blend (max "100%" power) (8/16/16)
        .set_level    = set_level_led34a_blend,
        .gradual_tick = gradual_tick_led34a_blend,
        .has_args     = 1
    },
    { // 3rd + 4th LEDs, manual blend (max "100%" power) (16/16/8)
        .set_level    = set_level_led34b_blend,
        .gradual_tick = gradual_tick_led34b_blend,
        .has_args     = 1
    },
    { // 3ch blend (HSV style)
        .set_level    = set_level_hsv,
        .gradual_tick = gradual_tick_hsv,
        .has_args     = 1
    },
    { // 3ch auto blend (red-warm-cool style, led4-led3-main2)
        .set_level    = set_level_auto3,
        .gradual_tick = gradual_tick_auto3,
        .has_args     = 0
    },
    RGB_AUX_CHANNELS
};


// LEDs 1+2 are 8-bit
// this 8-bit channel may be LEDs 1+2 or LED 4, depending on wiring
void set_level_main2(uint8_t level) {
    if (level == 0) {
        MAIN2_PWM_LVL = 0;
        MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);  // turn off opamp
        LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN );  // turn off opamp
        LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN );  // turn off opamp
    } else {
        level --;
        MAIN2_ENABLE_PORT |= (1 << MAIN2_ENABLE_PIN);
        MAIN2_PWM_LVL = PWM_GET8(pwm1_levels, level);
    }
}

// LED 3 is 16-bit
void set_level_led3(uint8_t level) {
    if (level == 0) {
        LED3_PWM_LVL = 0;
        PWM_CNT      = 0;
        MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);  // turn off opamp
        LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN );  // turn off opamp
        LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN );  // turn off opamp
    } else {
        level --;
        LED3_ENABLE_PORT |= (1 << LED3_ENABLE_PIN);
        LED3_PWM_LVL = PWM_GET16(pwm2_levels, level);
        uint16_t top = PWM_GET16(pwm_tops, level);
        while(actual_level && (PWM_CNT > (top - 32))) {}
        PWM_TOP = top;
        if (! actual_level) PWM_CNT = 0;
    }
}

// this 16-bit channel may be LED 4 or LEDs 1+2, depending on wiring
void set_level_led4(uint8_t level) {
    if (level == 0) {
        LED4_PWM_LVL = 0;
        PWM_CNT      = 0;  // reset phase
        MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);  // turn off opamp
        LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN );  // turn off opamp
        LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN );  // turn off opamp
    } else {
        level --;  // PWM array index = level - 1
        // gotta turn on the opamp before light can come out
        LED4_ENABLE_PORT |= (1 << LED4_ENABLE_PIN);
        LED4_PWM_LVL = PWM_GET16(pwm2_levels, level);
        // pulse frequency modulation, a.k.a. dynamic PWM
        uint16_t top = PWM_GET16(pwm_tops, level);
        // wait to sync the counter and avoid flashes
        while(actual_level && (PWM_CNT > (top - 32))) {}
        PWM_TOP = top;
        // force reset phase when turning on from zero
        // (because otherwise the initial response is inconsistent)
        if (! actual_level) PWM_CNT = 0;
    }
}

void set_level_all(uint8_t level) {
    if (level == 0) {
        MAIN2_PWM_LVL = 0;
        LED3_PWM_LVL  = 0;
        LED4_PWM_LVL  = 0;
        PWM_CNT       = 0;
        MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);  // turn off opamp
        LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN );  // turn off opamp
        LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN );  // turn off opamp
        return;
    }

    level --;

    MAIN2_ENABLE_PORT |= (1 << MAIN2_ENABLE_PIN);
    LED3_ENABLE_PORT  |= (1 << LED3_ENABLE_PIN );
    LED4_ENABLE_PORT  |= (1 << LED4_ENABLE_PIN );
    // FIXME? It might be better to calculate the 8-bit value from the
    //        16-bit tables instead of using the 8-bit ramp
    //        8bit = max(1, 16bit * 255 / top)
    MAIN2_PWM_LVL = PWM_GET8 (pwm1_levels, level);
    LED3_PWM_LVL  = PWM_GET16(pwm2_levels, level);
    LED4_PWM_LVL  = PWM_GET16(pwm2_levels, level);
    uint16_t top  = PWM_GET16(pwm_tops, level);
    while(actual_level && (PWM_CNT > (top - 32))) {}
    PWM_TOP = top;
    if (! actual_level) PWM_CNT = 0;
}

// 8/16/16 wiring, mix 16+16
void set_level_led34a_blend(uint8_t level) {
    if (level == 0) {
        LED3_PWM_LVL = 0;
        LED4_PWM_LVL = 0;
        PWM_CNT      = 0;  // reset phase
        MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);  // turn off opamp
        LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN );  // turn off opamp
        LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN );  // turn off opamp
        return;
    }

    level --;  // PWM array index = level - 1

    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET16(pwm2_levels, level);
    PWM_DATATYPE top = PWM_GET16(pwm_tops, level);
    uint8_t blend = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, top, blend);

    if (warm_PWM > 0) LED3_ENABLE_PORT |= (1 << LED3_ENABLE_PIN);
    if (cool_PWM > 0) LED4_ENABLE_PORT |= (1 << LED4_ENABLE_PIN);
    LED3_PWM_LVL = warm_PWM;
    LED4_PWM_LVL = cool_PWM;
    while(actual_level && (PWM_CNT > (top - 32))) {}
    PWM_TOP = top;
    if (! actual_level) PWM_CNT = 0;
}

// 16/16/8 wiring, mix 16+8
void set_level_led34b_blend(uint8_t level) {
    if (level == 0) {
        LED3_PWM_LVL = 0;
        LED4_PWM_LVL = 0;
        PWM_CNT      = 0;  // reset phase
        MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);  // turn off opamp
        LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN );  // turn off opamp
        LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN );  // turn off opamp
        return;
    }

    level --;

    const uint16_t top = 2047;
    uint16_t warm_PWM, cool_PWM;  // 11 bits, 8 bits
    uint8_t blend = cfg.channel_mode_args[channel_mode];
    uint16_t brightness = PWM_GET8(pwm1_levels, level);

    if (0 == brightness) brightness = 1;
    brightness = brightness << 3;

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, top, blend);

    // adjust to halfway between 8-bit steps
    warm_PWM -= 4;
    if (warm_PWM > top) warm_PWM = 0;

    if (cool_PWM > 0) MAIN2_ENABLE_PORT |= (1 << MAIN2_ENABLE_PIN);
    if (warm_PWM > 0) LED3_ENABLE_PORT  |= (1 <<  LED3_ENABLE_PIN);
    MAIN2_PWM_LVL = (uint8_t)(cool_PWM >> 3);
    LED3_PWM_LVL  = warm_PWM;
    //while(actual_level && (PWM_CNT > (top - 32))) {}
    PWM_TOP = top;
    if (! actual_level) PWM_CNT = 0;
}

void set_level_hsv(uint8_t level) {
    // TODO: implement a custom 3H handler which wraps around 0 to 255
    if (level == 0) {
        MAIN2_PWM_LVL = 0;
        LED3_PWM_LVL  = 0;
        LED4_PWM_LVL  = 0;
        PWM_CNT       = 0;
        MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);  // turn off opamp
        LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN );  // turn off opamp
        LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN );  // turn off opamp
        return;
    }

    level --;

    RGB_t color;
    uint8_t h = cfg.channel_mode_args[channel_mode];
    uint8_t s = 255;  // TODO: drop saturation at brightest levels
    uint8_t v = PWM_GET8(pwm1_levels, level);
    color = hsv2rgb(h, s, v);

    if (color.r > 0) MAIN2_ENABLE_PORT |= (1 << MAIN2_ENABLE_PIN);
    if (color.g > 0) LED3_ENABLE_PORT  |= (1 << LED3_ENABLE_PIN );
    if (color.b > 0) LED4_ENABLE_PORT  |= (1 << LED4_ENABLE_PIN );
    MAIN2_PWM_LVL = color.r;
    LED3_PWM_LVL  = color.g;
    LED4_PWM_LVL  = color.b;
    //while(actual_level && (PWM_CNT > (255 - 32))) {}
    PWM_TOP = 255;
    if (! actual_level) PWM_CNT = 0;
}

// calculate a 3-channel "auto tint" blend
// (like red -> warm white -> cool white)
// results are placed in *a, *b, and *c vars
// level : ramp level to convert into 3 channel levels
// (assumes ramp table is "pwm1_levels")
void calc_auto_3ch_blend(
    uint16_t *a,  // red
    uint16_t *b,  // warm
    uint8_t  *c,  // cool
    uint8_t level) {

    // led4=red, led3=warm
    uint16_t vpwm  = PWM_GET16(pwm2_levels, level);
    // main2=white, 8-bit
    uint8_t  vpwm8 = PWM_GET8 (pwm1_levels, level);

    // tint goes from 0 (red) to 127 (warm white) to 255 (cool white)
    uint8_t mytint;
    mytint = 255 * (uint16_t)level / RAMP_SIZE;

    // TODO: make "a" drop to zero sooner, and "c" start ramping up later
    // red is high at 0, low at 255 (linear)
    *a = (((PWM_DATATYPE2)(255 - mytint)
         * (PWM_DATATYPE2)vpwm) + 127) / 255;
    // warm white is low at 0 and 255, high at 127 (linear triangle)
    *b = (((PWM_DATATYPE2)triangle_wave(mytint)
         * (PWM_DATATYPE2)vpwm) + 127) / 255;
    // cool white is low at 0, high at 255 (linear)
    *c = (uint8_t)(
         (((PWM_DATATYPE2)mytint
         * (PWM_DATATYPE2)vpwm8) + 127) / 255
         );

}

// 3-channel "auto tint" channel mode
void set_level_auto3(uint8_t level) {
    if (level == 0) {
        MAIN2_PWM_LVL = 0;
        LED3_PWM_LVL  = 0;
        LED4_PWM_LVL  = 0;
        PWM_CNT       = 0;
        MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);  // turn off opamp
        LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN );  // turn off opamp
        LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN );  // turn off opamp
        return;
    }

    level --;

    uint16_t a, b;
    uint8_t c;
    calc_auto_3ch_blend(&a, &b, &c, level);

    // pulse frequency modulation, a.k.a. dynamic PWM
    uint16_t top = PWM_GET(pwm_tops, level);

    if (a > 0) LED4_ENABLE_PORT  |= (1 << LED4_ENABLE_PIN );
    else LED4_ENABLE_PORT  &= ~(1 << LED4_ENABLE_PIN );
    if (b > 0) LED3_ENABLE_PORT  |= (1 << LED3_ENABLE_PIN );
    else LED3_ENABLE_PORT  &= ~(1 << LED3_ENABLE_PIN );
    if (c > 0) MAIN2_ENABLE_PORT |= (1 << MAIN2_ENABLE_PIN);
    else MAIN2_ENABLE_PORT &= ~(1 << MAIN2_ENABLE_PIN);

    LED4_PWM_LVL  = a;  // red
    LED3_PWM_LVL  = b;  // warm
    MAIN2_PWM_LVL = c;  // cool

    while(actual_level && (PWM_CNT > (255 - 32))) {}
    PWM_TOP = top;
    if (! actual_level) PWM_CNT = 0;
}

///// "gradual tick" functions for smooth thermal regulation /////

bool gradual_adjust(uint8_t main2, uint16_t led3, uint16_t led4) {
    GRADUAL_ADJUST_SIMPLE(main2, MAIN2_PWM_LVL);
    GRADUAL_ADJUST_SIMPLE(led3,  LED3_PWM_LVL );
    GRADUAL_ADJUST_SIMPLE(led4,  LED4_PWM_LVL );

    if ((main2 == MAIN2_PWM_LVL)
     && (led3  == LED3_PWM_LVL )
     && (led4  == LED4_PWM_LVL )) {
        return true;  // done
    }
    return false;  // not done yet
}

bool gradual_tick_main2(uint8_t gt) {
    uint8_t main2 = PWM_GET8(pwm1_levels, gt);
    return gradual_adjust(main2, 0, 0);
}

bool gradual_tick_led3(uint8_t gt) {
    uint16_t led3 = PWM_GET16(pwm2_levels, gt);
    return gradual_adjust(0, led3, 0);
}

bool gradual_tick_led4(uint8_t gt) {
    uint16_t led4 = PWM_GET16(pwm2_levels, gt);
    return gradual_adjust(0, 0, led4);
}

bool gradual_tick_all(uint8_t gt) {
    uint8_t  main2 = PWM_GET8 (pwm1_levels, gt);
    uint16_t led3  = PWM_GET16(pwm2_levels, gt);
    uint16_t led4  = PWM_GET16(pwm2_levels, gt);
    return gradual_adjust(main2, led3, led4);
}

// 8/16/16 wiring, mix 16+16
bool gradual_tick_led34a_blend(uint8_t gt) {
    PWM_DATATYPE warm_PWM, cool_PWM;
    PWM_DATATYPE brightness = PWM_GET16(pwm2_levels, gt);
    PWM_DATATYPE top = PWM_GET16(pwm_tops, gt);
    uint8_t blend = cfg.channel_mode_args[channel_mode];

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, top, blend);

    return gradual_adjust(0, warm_PWM, cool_PWM);
}

// 16/16/8 wiring, mix 16+8
bool gradual_tick_led34b_blend(uint8_t gt) {
    const uint16_t top = 2047;
    uint16_t warm_PWM, cool_PWM;  // 11 bits, 8 bits
    uint8_t blend = cfg.channel_mode_args[channel_mode];
    uint16_t brightness = PWM_GET8(pwm1_levels, gt);

    if (0 == brightness) brightness = 1;
    brightness = brightness << 3;

    calc_2ch_blend(&warm_PWM, &cool_PWM, brightness, top, blend);

    // adjust to halfway between 8-bit steps
    warm_PWM -= 4;
    if (warm_PWM > top) warm_PWM = 0;

    // convert to 8-bit
    cool_PWM = (uint8_t)(cool_PWM >> 3);

    return gradual_adjust(cool_PWM, warm_PWM, 0);
}

bool gradual_tick_hsv(uint8_t gt) {
    // figure out what exact PWM levels we're aiming for
    RGB_t color;
    uint8_t h = cfg.channel_mode_args[channel_mode];
    uint8_t s = 255;  // TODO: drop saturation at brightest levels
    uint8_t v = PWM_GET8(pwm1_levels, gt);
    color = hsv2rgb(h, s, v);

    return gradual_adjust(color.r, color.g, color.b);
}

bool gradual_tick_auto3(uint8_t gt) {
    // figure out what exact PWM levels we're aiming for
    uint16_t red, warm;
    uint8_t cool;
    calc_auto_3ch_blend(&red, &warm, &cool, gt);
    return gradual_adjust(cool, warm, red);
}

