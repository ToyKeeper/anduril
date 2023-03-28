/*
 * tactical-mode.h: Tactical mode for Anduril.
 *
 * Copyright (C) 2023 Selene ToyKeeper
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

#ifndef TACTICAL_MODE_H
#define TACTICAL_MODE_H

// tactical(ish) mode
uint8_t tactical_state(Event event, uint16_t arg);

#ifndef TACTICAL_LEVELS
// high, low, tactical strobe
#define TACTICAL_LEVELS 120,30,(RAMP_SIZE+2)
#endif
uint8_t tactical_levels[];
uint8_t tactical_config_state(Event event, uint16_t arg);


#endif
