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
    } else {
        level --;
        #if PWM_CHANNELS >= 1
        PWM1_LVL = pgm_read_byte(pwm1_levels + level);
        #endif
        #if PWM_CHANNELS >= 2
        PWM2_LVL = pgm_read_byte(pwm2_levels + level);
        #endif
        #if PWM_CHANNELS >= 3
        PWM3_LVL = pgm_read_byte(pwm3_levels + level);
        #endif
        #if PWM_CHANNELS >= 4
        PWM4_LVL = pgm_read_byte(pwm4_levels + level);
        #endif
    }
    #ifdef USE_DYNAMIC_UNDERCLOCKING
    auto_clock_speed();
    #endif
}

#ifdef USE_SET_LEVEL_GRADUALLY
inline void set_level_gradually(uint8_t lvl) {
    gradual_target = lvl;
}

// call this every frame or every few frames to change brightness very smoothly
void gradual_tick() {
    // go by only one ramp level at a time instead of directly to the target
    uint8_t gt = gradual_target;
    if (gt < actual_level) gt = actual_level - 1;
    else if (gt > actual_level) gt = actual_level + 1;

    gt --;  // convert 1-based number to 0-based

    uint8_t target;

    #if PWM_CHANNELS >= 1
    target = pgm_read_byte(pwm1_levels + gt);
    if ((gt < actual_level)     // special case for FET-only turbo
            && (PWM1_LVL == 0)  // (bypass adjustment period for first step)
            && (target == 255)) PWM1_LVL = 255;
    else if (PWM1_LVL < target) PWM1_LVL ++;
    else if (PWM1_LVL > target) PWM1_LVL --;
    #endif
    #if PWM_CHANNELS >= 2
    target = pgm_read_byte(pwm2_levels + gt);
    if (PWM2_LVL < target) PWM2_LVL ++;
    else if (PWM2_LVL > target) PWM2_LVL --;
    #endif
    #if PWM_CHANNELS >= 3
    target = pgm_read_byte(pwm3_levels + gt);
    if (PWM3_LVL < target) PWM3_LVL ++;
    else if (PWM3_LVL > target) PWM3_LVL --;
    #endif
    #if PWM_CHANNELS >= 4
    target = pgm_read_byte(pwm4_levels + gt);
    if (PWM4_LVL < target) PWM4_LVL ++;
    else if (PWM4_LVL > target) PWM4_LVL --;
    #endif

    // did we go far enough to hit the next defined ramp level?
    // if so, update the main ramp level tracking var
    if ((PWM1_LVL == pgm_read_byte(pwm1_levels + gt))
        #if PWM_CHANNELS >= 2
            && (PWM2_LVL == pgm_read_byte(pwm2_levels + gt))
        #endif
        #if PWM_CHANNELS >= 3
            && (PWM3_LVL == pgm_read_byte(pwm3_levels + gt))
        #endif
        #if PWM_CHANNELS >= 4
            && (PWM4_LVL == pgm_read_byte(pwm4_levels + gt))
        #endif
        )
    {
        actual_level = gt + 1;
    }
}
#endif

#endif  // ifdef USE_RAMPING
#endif
