// fsm-ramping.h: Ramping functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef USE_RAMPING

// actual_level: last ramp level set by set_level()
uint8_t actual_level = 0;
// the level used before actual
uint8_t prev_level = 0;
#ifdef USE_RAMP_LEVEL_HARD_LIMIT
uint8_t ramp_level_hard_limit = 0;
#endif

void set_level(uint8_t level);
//void set_level_smooth(uint8_t level);
void set_level_zero();  // implement this in a hwdef

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
// FIXME: this whole thing is a mess and should be removed
#ifndef PWM1_BITS
    #define PWM1_BITS 8
    #define PWM1_TOP 255
    #define STACKED_PWM_TOP 255
#endif
#if PWM_BITS <= 8
    #define STACKED_PWM_DATATYPE uint8_t
    #define PWM_DATATYPE uint8_t
    #define PWM_DATATYPE2 uint16_t
    #ifndef PWM_TOP
        #define PWM_TOP 255
    #endif
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
#ifdef PWM4_LEVELS
PROGMEM const PWM4_DATATYPE pwm4_levels[] = { PWM4_LEVELS };
#endif
#ifdef PWM5_LEVELS
PROGMEM const PWM5_DATATYPE pwm5_levels[] = { PWM5_LEVELS };
#endif

// convenience defs for 1 LED with stacked channels
// FIXME: remove this, use pwm1/2/3 instead
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
// FIXME: remove this, use pwm1/2/3 instead
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

