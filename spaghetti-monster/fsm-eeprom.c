/*
 * fsm-eeprom.c: EEPROM API for SpaghettiMonster.
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

#ifndef FSM_EEPROM_C
#define FSM_EEPROM_C

#include "fsm-eeprom.h"

#if EEPROM_BYTES > 0
#if EEPROM_BYTES >= (EEPSIZE/2)
#error Requested EEPROM_BYTES too big.
#endif
uint8_t eeprom[EEPROM_BYTES];

uint8_t load_eeprom() {
    cli();
    // check if eeprom has been initialized; abort if it hasn't
    uint8_t marker = eeprom_read_byte((uint8_t *)EEP_START);
    if (marker != EEP_MARKER) { sei(); return 0; }

    // load the actual data
    for(uint8_t i=0; i<EEPROM_BYTES; i++) {
        eeprom[i] = eeprom_read_byte((uint8_t *)(EEP_START+1+i));
    }
    sei();
    return 1;
}

void save_eeprom() {
    cli();
    eeprom_update_byte((uint8_t *)EEP_START, EEP_MARKER);

    // save the actual data
    for(uint8_t i=0; i<EEPROM_BYTES; i++) {
        eeprom_update_byte((uint8_t *)(EEP_START+1+i), eeprom[i]);
    }
    sei();
}
#endif

#if EEPROM_WL_BYTES > 0
#if EEPROM_WL_BYTES >= (EEPSIZE/4)
#error Requested EEPROM_WL_BYTES too big.
#endif
uint8_t eeprom_wl[EEPROM_WL_BYTES];

uint8_t load_wl_eeprom() {
}

void save_wl_eeprom() {
}
#endif


#endif
