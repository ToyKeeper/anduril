// aux-leds.h: Aux LED functions for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#if defined(USE_AUX1_LED) && defined(TICK_DURING_STANDBY)
void aux1_led_update(uint8_t mode, uint8_t tick);
#endif
#if defined(USE_AUXRGB_LEDS) && defined(TICK_DURING_STANDBY)
uint8_t setting_rgb_mode_now = 0;
void rgb_led_update(uint8_t mode, uint16_t arg);
void rgb_led_voltage_readout(uint8_t power);
#ifdef USE_SMOOTH_POVD
RGB_t voltage_to_rgb_t (rgb_uint_t brightness);
#endif
#endif
/*
 * 0: R
 * 1: RG
 * 2:  G
 * 3:  GB
 * 4:   B
 * 5: R B
 * 6: RGB
 * 7: rainbow
 * 8: voltage
 */
const PROGMEM uint8_t rgb_led_colors[] = {
    0b00000000,  // 0: black
    0b00000001,  // 1: red
    0b00000101,  // 2: yellow
    0b00000100,  // 3: green
    0b00010100,  // 4: cyan
    0b00010000,  // 5: blue
    0b00010001,  // 6: purple
    0b00010101,  // 7: white
};
const PROGMEM uint8_t voltage_colors[] = {
    // voltage, color
        0, 0, // black
    #ifdef DUAL_VOLTAGE_FLOOR
    // AA / NiMH voltages
     8*dV, 0, // black
     9*dV, 1, // R
    10*dV, 2, // R+G
    11*dV, 3, //   G
    12*dV, 4, //   G+B
    13*dV, 5, //     B
    14*dV, 6, // R + B
    16*dV, 7, // R+G+B
    20*dV, 0, // black
    #endif
    // li-ion voltages
    29*dV, 0, // black
    30*dV, 1, // R
    33*dV, 2, // R+G
    35*dV, 3, //   G
    37*dV, 4, //   G+B
    39*dV, 5, //     B
    41*dV, 6, // R + B
    44*dV, 7, // R+G+B  // skip; looks too similar to G+B
      255, 7, // R+G+B
};

const PROGMEM uint8_t aux_animations[] = {
    // each line is: num_frames, then frame data
    // 2-level blink at ~0.84 Hz; is an odd length to avoid lining up with rainbow loop
    19, 2, 1, 0, 0,  0, 0, 0, 0,  0,  1, 0, 0, 0,  0, 0, 0, 0,  0, 1,
    // 2 blinks every 2 seconds  (1/8th power)
    16, 1, 0, 0, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    // 1 blink every 2 seconds  (1/16th power)
    16, 1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
};

typedef enum {
    aux_off_e = 0,
    aux_low_e,
    aux_high_e,
    #ifdef TICK_DURING_STANDBY
        aux_blinking_e,
        #ifdef USE_AUXRGB_LEDS
            // single-channel-only aux code doesn't support these
            // (because most of those lights are 8192 bytes only,
            //  and ROM size matters more than extra blinky patterns)
            aux_blinking_2_e,
            aux_blinking_3_e,
        #endif
    #endif
    aux_num_modes_e
} aux_modes_t;
typedef enum {
    aux_rgb_red_e = 0,
    aux_rgb_yellow_e,
    aux_rgb_green_e,
    aux_rgb_cyan_e,
    aux_rgb_blue_e,
    aux_rgb_purple_e,
    aux_rgb_white_e,
    aux_rgb_disco_e,
    aux_rgb_rainbow_e,
    aux_rgb_voltage_e,
    // extra copy, to make "voltage" easier to reach (at Hank's request)
    aux_rgb_voltage_again_e,
    aux_rgb_num_colors_e
} aux_rgb_colors_t;
//#define RGB_LED_NUM_COLORS  aux_rgb_num_colors_e
//#define RGB_LED_NUM_PATTERNS  aux_num_modes_e
#define aux1_cfg_byte(off,lockout)   ((lockout << 4) + off)
#define auxrgb_cfg_byte(mode,color)  ((mode << 4) + color)
#ifndef RGB_LED_OFF_DEFAULT
#define RGB_LED_OFF_DEFAULT  auxrgb_cfg_byte(aux_low_e, aux_rgb_voltage_e)
#endif
#ifndef RGB_LED_LOCKOUT_DEFAULT
#define RGB_LED_LOCKOUT_DEFAULT  auxrgb_cfg_byte(aux_blinking_e, aux_rgb_voltage_e)
#endif
#ifndef RGB_RAINBOW_SPEED
#define RGB_RAINBOW_SPEED 0x0f  // change color every 16 frames
#endif

//#define USE_OLD_BLINKING_INDICATOR
//#define USE_FANCIER_BLINKING_INDICATOR
#ifdef USE_AUX1_LED
    // bits 4-7 control lockout mode
    // bits 0-3 control "off" mode
    // modes are: 0=off, 1=low, 2=high, 3=blinking (if TICK_DURING_STANDBY enabled)
    //   (and maybe other modes, depending on hardware capabilities)
    #ifndef AUX1_DEFAULT_MODE
        #ifdef TICK_DURING_STANDBY
            #define AUX1_DEFAULT_MODE  aux1_cfg_byte(aux_low_e, aux_blinking_e)
        #else
            #define AUX1_DEFAULT_MODE  aux1_cfg_byte(aux_low_e, aux_low_e)
        #endif
    #endif
#endif

