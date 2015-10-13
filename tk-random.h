#ifndef TK_RANDOM_H
#define TK_RANDOM_H
/*
 * Smaller, pseudo-random function(s).
 *
 * Copyright (C) 2015 Selene ToyKeeper
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
 *
 */

uint8_t pgm_rand() {
    static uint8_t *offset = 0;
    return pgm_read_byte(offset++);
}

#endif  // TK_RANDOM_H
