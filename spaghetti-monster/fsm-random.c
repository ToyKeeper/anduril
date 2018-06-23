/*
 * fsm-random.c: Random number generator for SpaghettiMonster.
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

#ifndef FSM_RANDOM_C
#define FSM_RANDOM_C

#ifdef USE_PSEUDO_RAND
uint8_t pseudo_rand() {
    static uint16_t offset = 1024;
    // loop from 1024 to 4095
    offset = ((offset + 1) & 0x0fff) | 0x0400;
    pseudo_rand_seed += 0b01010101;  // 85
    return pgm_read_byte(offset) + pseudo_rand_seed;
}
#endif

#endif
