/*
 * fsm-eeprom.h: EEPROM API for SpaghettiMonster.
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

#ifndef FSM_EEPROM_H
#define FSM_EEPROM_H

#include <avr/eeprom.h>

// set this higher to enable normal eeprom functions
#ifndef EEPROM_BYTES
#define EEPROM_BYTES 0
#endif

// set this higher to enable wear-levelled eeprom functions
#ifndef EEPROM_WL_BYTES
#define EEPROM_WL_BYTES 0
#endif

#ifdef USE_EEPROM
// this fails when EEPROM_BYTES is a sizeof()
//#if EEPROM_BYTES >= (EEPSIZE/2)
//#error Requested EEPROM_BYTES too big.
//#endif
#ifdef EEPROM_OVERRIDE
uint8_t *eeprom;
#else
uint8_t eeprom[EEPROM_BYTES];
#endif
uint8_t load_eeprom();  // returns 1 for success, 0 for no data found
void save_eeprom();
#define EEP_START (EEPSIZE/2)
#endif

#ifdef USE_EEPROM_WL
#if EEPROM_WL_BYTES >= (EEPSIZE/4)
#error Requested EEPROM_WL_BYTES too big.
#endif
uint8_t eeprom_wl[EEPROM_WL_BYTES];
uint8_t load_eeprom_wl();  // returns 1 for success, 0 for no data found
void save_eeprom_wl();
#define EEP_WL_SIZE (EEPSIZE/2)
#endif

#if EEPSIZE > 256
#define EEP_OFFSET_T uint16_t
#else
#define EEP_OFFSET_T uint8_t
#endif

// if this marker isn't found, the eeprom is assumed to be blank
#define EEP_MARKER 0b10100101

#endif
