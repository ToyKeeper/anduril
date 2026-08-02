// aux-leds.c: Aux LED functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "anduril/aux-leds.h"


#if defined(USE_AUX1_LED) && (!defined(USE_AUXRGB_LEDS))
void aux1_led_update(uint8_t mode, uint8_t tick) {
    //uint8_t volts = voltage;  // save a few bytes by caching volatile value
    // turn off when battery is too low
    #ifdef DUAL_VOLTAGE_FLOOR
    if (((voltage < VOLTAGE_LOW) && (voltage > DUAL_VOLTAGE_FLOOR))
        || (voltage < DUAL_VOLTAGE_LOW_LOW)) {
    #else
    if (voltage < VOLTAGE_LOW) {
    #endif
        set_aux1_power(0);
    }
    //#ifdef USE_AUX1LOW_BAT_WARNING
    #ifndef DUAL_VOLTAGE_FLOOR // this isn't set up for dual-voltage lights like the Sofirn SP10 Pro
    // fast blink a warning when battery is low but not critical
    else if (voltage < VOLTAGE_RED) {
        set_aux1_power(mode & (((tick & 0b0010)>>1) - 3));
    }
    #endif
    //#endif
    // normal steady output, 0/1/2 = off / low / high
    else if (mode < 3) {
        set_aux1_power(mode);
    }
    // beacon-like blinky mode
    else {
        #ifdef USE_OLD_BLINKING_INDICATOR

        // basic blink, 1/8th duty cycle
        if (! (tick & 7)) {
            set_aux1_power(2);
        }
        else {
            set_aux1_power(0);
        }

        #else

        // fancy blink, set off/low/high levels here:
        static const uint8_t seq[] = {0, 1, 2, 1,  0, 0, 0, 0,
                                      0, 0, 1, 0,  0, 0, 0, 0};
        set_aux1_power(seq[tick & 15]);

        #endif  // ifdef USE_OLD_BLINKING_INDICATOR
    }
}
#endif

#if defined(USE_AUXRGB_LEDS) && defined(TICK_DURING_STANDBY)
uint8_t voltage_to_rgb() {
    uint8_t volts = voltage;
    //if (volts < VOLTAGE_LOW) return 0;

    uint8_t i;
    for (i = 0;  volts >= pgm_read_byte(voltage_colors + i);  i += 2) {}
    uint8_t color_num = pgm_read_byte(voltage_colors + (i - 2) + 1);
    return pgm_read_byte(rgb_led_colors + color_num);
}

#ifdef USE_SMOOTH_POVD
RGB_t voltage_to_rgb_t (rgb_uint_t brightness) {
    // calculate in-between voltage colors
    // by doing linear interpolation between voltage_colors[] entries

    // adjust down slightly to better match non-smooth povd colors
    uint8_t volts = voltage - (voltage / (6*dV));

    RGB_t color;
    uint8_t i;
    for (i = 0;  volts >= pgm_read_byte(voltage_colors + i);  i += 2) {}
    uint8_t voltage_low   = pgm_read_byte(voltage_colors + (i - 2));
    uint8_t color_num_low = pgm_read_byte(voltage_colors + (i - 2) + 1);
    uint8_t voltage_hi    = pgm_read_byte(voltage_colors + (i + 0));
    uint8_t color_num_hi  = pgm_read_byte(voltage_colors + (i + 0) + 1);
    uint8_t color_low = pgm_read_byte(rgb_led_colors + color_num_low);
    uint8_t color_hi  = pgm_read_byte(rgb_led_colors + color_num_hi);
    // 0 to N-1 where 0 = low color and N = hi color
    // (N is 5 minimum, or 20 max usually, but may occasionally be 50+)
    uint8_t steps = voltage_hi - voltage_low;
    rgb_uint_t levels_per_step = brightness / steps;
    if (brightness && (! levels_per_step)) levels_per_step = 1;
    uint8_t ratio = volts - voltage_low;
    color.r = (levels_per_step * ratio * (color_hi & 0b00000001))
        + (levels_per_step * (steps - ratio) * (color_low & 0b00000001));
    color.g = (levels_per_step * ratio * ((color_hi & 0b00000100) >> 2))
        + (levels_per_step * (steps - ratio) * ((color_low & 0b00000100) >> 2));
    color.b = (levels_per_step * ratio * ((color_hi & 0b00010000) >> 4))
        + (levels_per_step * (steps - ratio) * ((color_low & 0b00010000) >> 4));

    // scale to requested brightness
    //color.r = (uint16_t)color.r * brightness / RGB_MAX;
    //color.g = (uint16_t)color.g * brightness / RGB_MAX;
    //color.b = (uint16_t)color.b * brightness / RGB_MAX;

    return color;
}
#endif

// do fancy stuff with the RGB aux LEDs
// mode: 0bPPPPCCCC where PPPP is the pattern and CCCC is the color
// arg: time slice number
void rgb_led_update(uint8_t mode, uint16_t arg) {
    static uint8_t rainbow = 0;  // track state of rainbow mode
    static uint8_t frame = 0;  // track state of animation mode

    // turn off aux LEDs when battery is empty
    // (but if voltage==0, that means we just booted and don't know yet)
    uint8_t volts = voltage;  // save a few bytes by caching volatile value
    #ifdef DUAL_VOLTAGE_FLOOR
    if ((volts) && (((voltage < VOLTAGE_LOW) && (voltage > DUAL_VOLTAGE_FLOOR)) || (voltage < DUAL_VOLTAGE_LOW_LOW))) {
    #else
    if ((volts) && (volts < VOLTAGE_LOW)) {
    #endif
        set_auxrgb_power(0);
        #ifdef USE_AUX1_LED
        set_aux1_power(0);
        #endif
        return;
    }

    uint8_t pattern = (mode>>4);  // off, low, high, blinking, ... more?
    uint8_t color = mode & 0x0f;

    // always preview in high mode
    if (setting_rgb_mode_now) { pattern = 2; }

    #if defined(USE_POST_OFF_VOLTAGE) && (!defined(USE_SMOOTH_POVD))
    // use voltage high mode for a few seconds after initial poweroff
    // (but not after changing aux LED settings and other similar actions)
    else if ((arg < (cfg.post_off_voltage * SLEEP_TICKS_PER_SECOND))
          && (ticks_since_on < (cfg.post_off_voltage * SLEEP_TICKS_PER_SECOND))
          && (ticks_since_on > 0)  // don't blink red on 1st frame
        ) {
        // use high mode if regular aux level is high or prev level was high
        #ifdef USE_AUX_THRESHOLD_CONFIG
            // always high if configured for high aux
            // otherwise 0/1/2 depending on recent main LED brightness
            // (using >= makes it off by 1, but allows POVD at boot time)
            if (pattern != 2)
                pattern = (prev_level >= cfg.aux_low_ramp_level)
                       << (prev_level > cfg.aux_high_ramp_level);
        #else
            pattern = 1
                + ((2 == pattern)
                   | (prev_level >= POST_OFF_VOLTAGE_BRIGHTNESS));
        #endif
        // voltage mode
        color = aux_rgb_voltage_e;
    }
    #endif

    const uint8_t *colors = rgb_led_colors + 1;
    uint8_t actual_color = 0;
    if (color <= aux_rgb_white_e) {  // normal color
        actual_color = pgm_read_byte(colors + color);
    }
    else if (color == aux_rgb_disco_e) {  // disco
        rainbow = (rainbow + 1 + pseudo_rand() % 5) % 6;
        actual_color = pgm_read_byte(colors + rainbow);
    }
    else if (color == aux_rgb_rainbow_e) {  // rainbow
        uint8_t speed = 0x03;  // awake speed
        if (go_to_standby) speed = RGB_RAINBOW_SPEED;  // asleep speed
        if (0 == (arg & speed)) {
            rainbow = (rainbow + 1) % 6;
        }
        actual_color = pgm_read_byte(colors + rainbow);
    }
    else {  // voltage
        // show actual voltage while asleep...
        if (go_to_standby) {
            // choose a color based on battery voltage
            actual_color = voltage_to_rgb();
        }
        // ... but during preview, cycle colors quickly
        else {
            actual_color = pgm_read_byte(colors + (((arg>>1) % 3) << 1));
        }
    }

    // pick a brightness from the animation sequence
    if (pattern >= aux_blinking_e) {
        // find correct animation
        uint8_t base = 0;
        for (uint8_t f = aux_blinking_e;  f < pattern;  f++) {
            base = base + 1 + pgm_read_byte(aux_animations + base);
        }
        // display next frame
        uint8_t num_frames = pgm_read_byte(aux_animations + base);
        frame = (frame + 1) % num_frames;
        pattern = pgm_read_byte(aux_animations + base + frame + 1);
    }
    uint8_t result;
    #ifdef USE_AUX1_LED
    uint8_t button_led_result;
    #endif
    switch (pattern) {
        case 0:  // off
            result = 0;
            #ifdef USE_AUX1_LED
            button_led_result = 0;
            #endif
            break;
        case 1:  // low
            result = actual_color;
            #ifdef USE_AUX1_LED
            button_led_result = 1;
            #endif
            break;
        default:  // high
            result = (actual_color << 1);
            #ifdef USE_AUX1_LED
            button_led_result = 2;
            #endif
            break;
    }
    set_auxrgb_power(result);
    #ifdef USE_AUX1_LED
    set_aux1_power(button_led_result);
    #endif
}

void rgb_led_voltage_readout(uint8_t power) {
    switch(power) {
        case 0:  // off
            set_auxrgb_power(0);
            break;
        case 1:  // low
            set_auxrgb_power(voltage_to_rgb());
            break;
        default:  // high
            set_auxrgb_power(voltage_to_rgb() << 1);
            break;
    }
}
#endif

