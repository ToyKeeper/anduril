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
}

#ifdef USE_SET_LEVEL_GRADUALLY
inline void set_level_gradually(uint8_t lvl) {
    gradual_target = lvl;
}

// FIXME: This sucks
// FIXME: I think this can behave badly in some circumstances:
//        - large smooth adjustment in progress
//        - from something like (255,10) to (200, 0)
//        - ... and a new target gets set while actual_level is ... not quite right
//        - ... and it then ends up massively overshooting or even turning off
//        (still haven't figured out the exact circumstances to cause it)
//        (at the very least, actual_level can suddenly jump when multiple PWM levels are being adjusted)
//        (so it's bad to rely on actual_level in the middle of a smooth adjustment)
void gradual_tick() {
    int8_t changed = 0;
    uint8_t target;
    uint8_t gt = gradual_target - 1;
    // FIXME: rewrite this completely, probably
    //        (perhaps go by only one ramp level at a time instead of directly to the target?)
    #if PWM_CHANNELS >= 1
    target = pgm_read_byte(pwm1_levels + gt);
    if (PWM1_LVL < target) { PWM1_LVL ++; changed = 1; }
    else if (PWM1_LVL > target) { PWM1_LVL --; changed = -1; }
    #endif
    #if PWM_CHANNELS >= 2
    target = pgm_read_byte(pwm2_levels + gt);
    if (PWM2_LVL < target) { PWM2_LVL ++; changed = 1; }
    else if (PWM2_LVL > target) { PWM2_LVL --; changed = -1; }
    #endif
    #if PWM_CHANNELS >= 3
    target = pgm_read_byte(pwm3_levels + gt);
    if (PWM3_LVL < target) { PWM3_LVL ++; changed = 1; }
    else if (PWM3_LVL > target) { PWM3_LVL --; changed = -1; }
    #endif
    #if PWM_CHANNELS >= 4
    target = pgm_read_byte(pwm4_levels + gt);
    if (PWM4_LVL < target) { PWM4_LVL ++; changed = 1; }
    else if (PWM4_LVL > target) { PWM4_LVL --; changed = -1; }
    #endif
    // figure out what actual level is
    /*
    if (changed < 0) {
        #if PWM_CHANNELS >= 4
        if (PWM4_LVL < pgm_read_byte(pwm4_levels + actual_level - 1))
            actual_level --;
        else
        #endif
        #if PWM_CHANNELS >= 3
        if (PWM3_LVL < pgm_read_byte(pwm3_levels + actual_level - 1))
            actual_level --;
        else
        #endif
        #if PWM_CHANNELS >= 2
        if (PWM2_LVL < pgm_read_byte(pwm2_levels + actual_level - 1))
            actual_level --;
        else
        #endif
            if (PWM1_LVL < pgm_read_byte(pwm1_levels + actual_level - 1))
                actual_level --;
    }
    else if (changed > 0) {
        #if PWM_CHANNELS >= 4
        if (PWM4_LVL > pgm_read_byte(pwm4_levels + actual_level - 1))
            actual_level ++;
        else
        #endif
        #if PWM_CHANNELS >= 3
        if (PWM3_LVL > pgm_read_byte(pwm3_levels + actual_level - 1))
            actual_level ++;
        else
        #endif
        #if PWM_CHANNELS >= 2
        if (PWM2_LVL > pgm_read_byte(pwm2_levels + actual_level - 1))
            actual_level ++;
        else
        #endif
            if (PWM1_LVL > pgm_read_byte(pwm1_levels + actual_level - 1))
                actual_level ++;
    }
    else {
        actual_level = gradual_target;
    }
    */
    /*
    if (changed) {
        #if PWM_CHANNELS >= 4
        if (PWM4_LVL != pgm_read_byte(pwm4_levels + actual_level - 1))
            actual_level += changed;
        else
        #endif
        #if PWM_CHANNELS >= 3
        if (PWM3_LVL != pgm_read_byte(pwm3_levels + actual_level - 1))
            actual_level += changed;
        else
        #endif
        #if PWM_CHANNELS >= 2
        if (PWM2_LVL != pgm_read_byte(pwm2_levels + actual_level - 1))
            actual_level += changed;
        else
        #endif
            if (PWM1_LVL != pgm_read_byte(pwm1_levels + actual_level - 1))
                actual_level += changed;
    } else {
        actual_level = gradual_target;
    }
    */
    //if (! changed) { actual_level = gradual_target; }
    if (changed) {
        #if PWM_CHANNELS >= 4
        if (PWM4_LVL > 0) {
            for (actual_level = 0;
                 (actual_level < MAX_LEVEL) &&
                 (PWM4_LVL <= pgm_read_byte(pwm4_levels + actual_level - 1));
                 actual_level ++) {}
        } else
        #endif
        #if PWM_CHANNELS >= 3
        if (PWM3_LVL > 0) {
            for (actual_level = 0;
                 (actual_level < MAX_LEVEL) &&
                 (PWM3_LVL <= pgm_read_byte(pwm3_levels + actual_level - 1));
                 actual_level ++) {}
        } else
        #endif
        #if PWM_CHANNELS >= 2
        if (PWM2_LVL > 0) {
            for (actual_level = 0;
                 (actual_level < MAX_LEVEL) &&
                 (PWM2_LVL <= pgm_read_byte(pwm2_levels + actual_level - 1));
                 actual_level ++) {}
        } else
        #endif
        for (actual_level = 0;
             (actual_level < MAX_LEVEL) &&
             (PWM1_LVL <= pgm_read_byte(pwm1_levels + actual_level - 1));
             actual_level ++) {}
    } else {
        actual_level = gradual_target;
    }
}
#endif

// TODO: set_lvl_smooth?

#endif  // ifdef USE_RAMPING
#endif
