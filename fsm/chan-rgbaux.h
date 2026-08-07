// channel modes for RGB aux LEDs
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define USE_CHANNEL_FLAGS

#define AUXRGB_CM_ENUMS  \
    CM_AUXRED, \
    CM_AUXYEL, \
    CM_AUXGRN, \
    CM_AUXCYN, \
    CM_AUXBLU, \
    CM_AUXPRP, \
    CM_AUXWHT

#define AUXRGB_CM_ARGS  0,0,0,0,0,0,0

#define NUM_AUXRGB_CHANNEL_MODES  7

// include / exclude field based on compile options
#ifdef USE_CHANNEL_FLAGS
    #define AUXRGB_FLAGS , .flags = CHANNEL_FLAG_IS_AUX
#else
    #define AUXRGB_FLAGS
#endif

#define AUXRGB_CHANNELS \
    { \
        .set_level    = set_level_auxred, \
        .gradual_tick = gradual_tick_null \
        AUXRGB_FLAGS \
    }, \
    { \
        .set_level    = set_level_auxyel, \
        .gradual_tick = gradual_tick_null \
        AUXRGB_FLAGS \
    }, \
    { \
        .set_level    = set_level_auxgrn, \
        .gradual_tick = gradual_tick_null \
        AUXRGB_FLAGS \
    }, \
    { \
        .set_level    = set_level_auxcyn, \
        .gradual_tick = gradual_tick_null \
        AUXRGB_FLAGS \
    }, \
    { \
        .set_level    = set_level_auxblu, \
        .gradual_tick = gradual_tick_null \
        AUXRGB_FLAGS \
    }, \
    { \
        .set_level    = set_level_auxprp, \
        .gradual_tick = gradual_tick_null \
        AUXRGB_FLAGS \
    }, \
    { \
        .set_level    = set_level_auxwht, \
        .gradual_tick = gradual_tick_null \
        AUXRGB_FLAGS \
    }

void set_level_auxred(uint8_t level);
void set_level_auxyel(uint8_t level);
void set_level_auxgrn(uint8_t level);
void set_level_auxcyn(uint8_t level);
void set_level_auxblu(uint8_t level);
void set_level_auxprp(uint8_t level);
void set_level_auxwht(uint8_t level);

bool gradual_tick_null(uint8_t gt);

