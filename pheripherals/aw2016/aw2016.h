// aw2016.h: Device driver for AW2016 pheripheral 
// Copyright (C) 2025 - 2026 Loneoceans, stcarlso
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AW2016_H

#define AW2016_H
#define AW2016_C  pheripherals/aw2016/aw2016.c
#define AW2016_ADDR 0x64

#include <stdint.h>

// Global current setting
typedef enum {
    AW2016_ISET_15mA = 0b00,
    AW2016_ISET_30mA = 0b01,
    AW2016_ISET_5mA  = 0b10,
    AW2016_ISET_10mA = 0b11
} aw2016_current_t;

extern uint8_t adv_rgb_do_not_sleep;

// public API
inline void aw2016_hwdef_setup ();
void aw2016_init(void);
void aw2016_set_rgb_calibration(uint8_t r, uint8_t g, uint8_t b);

void aw2016_set_global_current(aw2016_current_t cur);
void aw2016_set_manual_pattern_mode(uint8_t channel, uint8_t mode);
void aw2016_set_global_brightness(uint8_t pwm);

void aw2016_set_led(uint8_t channel, uint8_t pwm);
void aw2016_set_rgb(uint8_t r, uint8_t g, uint8_t b);
void aw2016_set_hsv(uint8_t h, uint8_t s, uint8_t v);

void aw2016_sleep(void);
void aw2016_wake(void);
void aw2016_enable_leds(void);
void aw2016_disable_leds(void);

// pre-built effects
void aw2016_test(void);
void aw2016_rainbow_rgb(uint8_t pwm);
void aw2016_rainbow_blend(uint8_t pwm);

#endif