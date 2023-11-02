// fsm-channels.h: Channel mode functions for SpaghettiMonster.
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// always enable channel modes, even if there is only one
#define USE_CHANNEL_MODES

// typedefs
typedef void SetLevelFunc(uint8_t level);
typedef SetLevelFunc * SetLevelFuncPtr;

typedef bool GradualTickFunc(uint8_t gt);
typedef GradualTickFunc * GradualTickFuncPtr;

// TODO: implement custom 3H handlers
typedef void ChannelArgFunc();
typedef ChannelArgFunc * ChannelArgFuncPtr;

typedef struct Channel {
    SetLevelFuncPtr set_level;
    #ifdef USE_SET_LEVEL_GRADUALLY
        GradualTickFuncPtr gradual_tick;
    #endif
    #ifdef USE_CUSTOM_3H_HANDLERS
        // TODO: implement custom 3H handlers
        ChannelArgFuncPtr ramp_channel_arg;
    #endif
    #ifdef USE_CHANNEL_MODE_ARGS
        bool has_args;
        //uint8_t arg;  // is in the config struct, not here
    #endif
} Channel;

Channel channels[];  // values are defined in the hwdef-*.c

// TODO: size-optimize the case with only 1 channel mode?
// (the arrays and stuff shouldn't be needed)

#if NUM_CHANNEL_MODES > 1
    #define USE_CHANNEL_MODES
    // current multi-channel mode
    uint8_t channel_mode = DEFAULT_CHANNEL_MODE;
#else
    #define channel_mode 0
#endif

#ifdef USE_CUSTOM_CHANNEL_3H_MODES
// different 3H behavior per channel?
// TODO: move to progmem
// TODO: move to Anduril, not FSM
StatePtr channel_3H_modes[NUM_CHANNEL_MODES];
#endif

//#ifdef USE_CHANNEL_MODE_TOGGLES
#if NUM_CHANNEL_MODES > 1
// user can take unwanted modes out of the rotation
// bitmask
#ifdef USE_CFG
    #define channel_mode_enabled(n) ((cfg.channel_modes_enabled >> n) & 1)
    #define channel_mode_enable(n)  cfg.channel_modes_enabled |= (1 << n)
    #define channel_mode_disable(n) cfg.channel_modes_enabled &= ((1 << n) ^ 0xff)
#else
    uint16_t channel_modes_enabled = CHANNEL_MODES_ENABLED;
    #define channel_mode_enabled(n) ((channel_modes_enabled >> n) & 1)
    #define channel_mode_enable(n)  channel_modes_enabled |= (1 << n)
    #define channel_mode_disable(n) channel_modes_enabled &= ((1 << n) ^ 0xff)
    #endif
#endif

#ifdef USE_CHANNEL_MODE_ARGS
    #ifndef USE_CFG
    // one byte of extra data per channel mode, like for tint value
    uint8_t channel_mode_args[NUM_CHANNEL_MODES] = { CHANNEL_MODE_ARGS };
    #endif
    // which modes respond to their "arg", and which don't?
    //const uint8_t channel_has_args = CHANNEL_HAS_ARGS;
    //#define channel_has_args(n) ((CHANNEL_HAS_ARGS >> n) & 1)
    // struct member
    #define channel_has_args(n) (channels[n].has_args)
#endif

#if NUM_CHANNEL_MODES > 1
void set_channel_mode(uint8_t mode);
#endif

#ifdef USE_CALC_2CH_BLEND
void calc_2ch_blend(
    PWM_DATATYPE *warm,
    PWM_DATATYPE *cool,
    PWM_DATATYPE brightness,
    PWM_DATATYPE top,
    uint8_t blend);
#endif

#ifdef USE_HSV2RGB
typedef struct RGB_t {
    uint16_t r;
    uint16_t g;
    uint16_t b;
} RGB_t;
RGB_t hsv2rgb(uint8_t h, uint8_t s, uint16_t v);
#endif  // ifdef USE_HSV2RGB


#ifdef USE_SET_LEVEL_1CH
// TODO: remove this
void set_level_1ch(uint8_t level);
#endif

#ifdef USE_SET_LEVEL_2CH_STACKED
// TODO: remove this
void set_level_2ch_stacked(uint8_t level);
#endif

#ifdef USE_SET_LEVEL_3CH_STACKED
// TODO: remove this
void set_level_3ch_stacked(uint8_t level);
#endif

#if defined(USE_TINT_RAMPING) && (!defined(TINT_RAMP_TOGGLE_ONLY))
// TODO: remove this
void set_level_2ch_blend();
#endif

#ifdef USE_GRADUAL_TICK_1CH
// TODO: remove this
void gradual_tick_1ch();
#endif

#ifdef USE_GRADUAL_TICK_2CH_STACKED
// TODO: remove this
void gradual_tick_2ch_stacked();
#endif

#ifdef USE_GRADUAL_TICK_3CH_STACKED
// TODO: remove this
void gradual_tick_3ch_stacked();
#endif

