/*
 * config-mode.h: Config mode base functions for Anduril.
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

#ifndef CONFIG_MODE_H
#define CONFIG_MODE_H

// config menu
uint8_t config_state_base(
    Event event,
    uint16_t arg,
    uint8_t num_config_steps,
    void (*savefunc)(uint8_t step, uint8_t value)
    );


#endif
