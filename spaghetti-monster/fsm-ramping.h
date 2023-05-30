// fsm-ramping.h: Ramping functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef USE_RAMPING

// actual_level: last ramp level set by set_level()
uint8_t actual_level = 0;
// the level used before actual
uint8_t prev_level = 0;

// TODO: size-optimize the case with only 1 channel mode
// (the arrays and stuff shouldn't be needed)

#ifdef USE_CFG
    #define CH_MODE cfg.channel_mode
#else
    // current multi-channel mode
    uint8_t channel_mode = DEFAULT_CHANNEL_MODE;
    #define CH_MODE channel_mode
#endif

#if NUM_CHANNEL_MODES > 1
#define USE_CHANNEL_MODES
#endif

// one function per channel mode
typedef void SetLevelFunc(uint8_t level);
typedef SetLevelFunc * SetLevelFuncPtr;
// TODO: move to progmem
SetLevelFuncPtr channel_modes[NUM_CHANNEL_MODES];

#ifdef USE_SET_LEVEL_GRADUALLY
// the gradual tick mechanism may be different per channel
typedef bool GradualTickFunc(uint8_t gt);
typedef GradualTickFunc * GradualTickFuncPtr;
// TODO: move to progmem
GradualTickFuncPtr gradual_tick_modes[NUM_CHANNEL_MODES];
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
    uint8_t channel_modes_enabled = CHANNEL_MODES_ENABLED;
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
    // bitmask: which modes respond to their "arg", and which don't?
    //const uint8_t channel_has_args = CHANNEL_HAS_ARGS;
    #define channel_has_args(n) ((CHANNEL_HAS_ARGS >> n) & 1)
#endif

void set_channel_mode(uint8_t mode);

void set_level(uint8_t level);
//void set_level_smooth(uint8_t level);

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
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;
RGB_t hsv2rgb(uint8_t h, uint8_t s, uint8_t v);
#endif  // ifdef USE_HSV2RGB

#ifdef USE_SET_LEVEL_GRADUALLY
// adjust brightness very smoothly
uint8_t gradual_target;
inline void set_level_gradually(uint8_t lvl);
void gradual_tick();

// reduce repetition with macros
#define GRADUAL_TICK_SETUP()  \
    PWM_DATATYPE target;

// tick to a specific value
#define GRADUAL_ADJUST_SIMPLE(TARGET,PWM)  \
    if (PWM < TARGET) PWM ++;  \
    else if (PWM > TARGET) PWM --;

// tick to a specific value, except when immediate 0 to 255 is needed
#define GRADUAL_ADJUST_STACKED(TARGET,PWM,TOP)  \
    if (   ((PWM ==   0) && (TARGET == TOP))  \
        || ((PWM == TOP) && (TARGET ==   0)))  \
        PWM = TARGET;  \
    else GRADUAL_ADJUST_SIMPLE(TARGET,PWM)

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

#endif  // ifdef USE_SET_LEVEL_GRADUALLY

// auto-detect the data type for PWM tables
// FIXME: PWM bits and data type should be per PWM table
#ifndef PWM1_BITS
    #define PWM1_BITS 8
    #define PWM1_TOP 255
    #define STACKED_PWM_TOP 255
#endif
#if PWM_BITS <= 8
    #define STACKED_PWM_DATATYPE uint8_t
    #define PWM_DATATYPE uint8_t
    #define PWM_DATATYPE2 uint16_t
    #define PWM_TOP 255
    #define STACKED_PWM_TOP 255
    #ifndef PWM_GET
    #define PWM_GET(x,y) pgm_read_byte(x+y)
    #endif
#else
    #define STACKED_PWM_DATATYPE uint16_t
    #define PWM_DATATYPE uint16_t
    #ifndef PWM_DATATYPE2
        #define PWM_DATATYPE2 uint32_t
    #endif
    #ifndef PWM_TOP
        #define PWM_TOP 1023  // 10 bits by default
    #endif
    #ifndef STACKED_PWM_TOP
        #define STACKED_PWM_TOP 1023
    #endif
    // pointer plus 2*y bytes
    //#define PWM_GET(x,y) pgm_read_word(x+(2*y))
    // nope, the compiler was already doing the math correctly
    #ifndef PWM_GET
    #define PWM_GET(x,y) pgm_read_word(x+y)
    #endif
#endif
#define PWM_GET8(x,y)  pgm_read_byte(x+y)
#define PWM_GET16(x,y) pgm_read_word(x+y)

// use UI-defined ramp tables if they exist
#ifdef PWM1_LEVELS
PROGMEM const PWM1_DATATYPE pwm1_levels[] = { PWM1_LEVELS };
#endif
#ifdef PWM2_LEVELS
PROGMEM const PWM2_DATATYPE pwm2_levels[] = { PWM2_LEVELS };
#endif
#ifdef PWM3_LEVELS
PROGMEM const PWM3_DATATYPE pwm3_levels[] = { PWM3_LEVELS };
#endif

// convenience defs for 1 LED with stacked channels
#ifdef LOW_PWM_LEVELS
PROGMEM const PWM_DATATYPE low_pwm_levels[]  = { LOW_PWM_LEVELS };
#endif
#ifdef MED_PWM_LEVELS
PROGMEM const PWM_DATATYPE med_pwm_levels[]  = { MED_PWM_LEVELS };
#endif
#ifdef HIGH_PWM_LEVELS
PROGMEM const PWM_DATATYPE high_pwm_levels[] = { HIGH_PWM_LEVELS };
#endif

// 2 channel CCT blending ramp
#ifdef BLEND_PWM_LEVELS
PROGMEM const PWM_DATATYPE blend_pwm_levels[] = { BLEND_PWM_LEVELS };
#endif


// pulse frequency modulation, a.k.a. dynamic PWM
// (different ceiling / frequency at each ramp level)
// FIXME: dynamic PWM should be a per-channel option, not global
#ifdef PWM_TOPS
PROGMEM const PWM_DATATYPE pwm_tops[] = { PWM_TOPS };
#endif

// FIXME: jump start should be per channel / channel mode
#ifdef USE_JUMP_START
    #ifndef JUMP_START_TIME
        #define JUMP_START_TIME 8  // in ms, should be 4, 8, or 12
    #endif
    #ifndef DEFAULT_JUMP_START_LEVEL
        #define DEFAULT_JUMP_START_LEVEL 10
    #endif
    #ifdef USE_CFG
        #define JUMP_START_LEVEL cfg.jump_start_level
    #else
        #define JUMP_START_LEVEL jump_start_level
        uint8_t jump_start_level = DEFAULT_JUMP_START_LEVEL;
    #endif
#endif

// RAMP_SIZE / MAX_LVL
// cfg-*.h should define RAMP_SIZE
//#define RAMP_SIZE (sizeof(stacked_pwm1_levels)/sizeof(STACKED_PWM_DATATYPE))
#define MAX_LEVEL RAMP_SIZE


#endif  // ifdef USE_RAMPING

