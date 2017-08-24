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

// TODO: set_lvl_smooth?

#endif  // ifdef USE_RAMPING
#endif
