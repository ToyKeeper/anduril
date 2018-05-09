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
#ifdef EEPROM_OVERRIDE
uint8_t *eeprom;
#else
uint8_t eeprom[EEPROM_BYTES];
#endif

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

    // save the actual data
    for(uint8_t i=0; i<EEPROM_BYTES; i++) {
        eeprom_update_byte((uint8_t *)(EEP_START+1+i), eeprom[i]);
    }

    // save the marker last, to indicate the transaction is complete
    eeprom_update_byte((uint8_t *)EEP_START, EEP_MARKER);
    sei();
}
#endif

#if EEPROM_WL_BYTES > 0
uint8_t eeprom_wl[EEPROM_WL_BYTES];
EEP_OFFSET_T eep_wl_prev_offset;

uint8_t load_eeprom_wl() {
    cli();
    // check if eeprom has been initialized; abort if it hasn't
    uint8_t found = 0;
    EEP_OFFSET_T offset;
    for(offset = 0;
        offset < EEP_WL_SIZE - EEPROM_WL_BYTES - 1;
        offset += (EEPROM_WL_BYTES + 1)) {
        if (eeprom_read_byte((uint8_t *)offset) == EEP_MARKER) {
            found = 1;
            eep_wl_prev_offset = offset;
            break;
        }
    }

    if (found) {
        // load the actual data
        for(uint8_t i=0; i<EEPROM_WL_BYTES; i++) {
            eeprom_wl[i] = eeprom_read_byte((uint8_t *)(offset+1+i));
        }
    }
    sei();
    return found;
}

void save_eeprom_wl() {
    cli();
    // erase old state
    EEP_OFFSET_T offset = eep_wl_prev_offset;
    for (uint8_t i = 0; i < EEPROM_WL_BYTES+1; i ++) {
        eeprom_update_byte((uint8_t *)offset+i, 0xFF);
    }

    // save new state
    offset += EEPROM_WL_BYTES+1;
    if (offset > EEP_WL_SIZE-EEPROM_WL_BYTES-1) offset = 0;
    eep_wl_prev_offset = offset;
    // marker byte
    // FIXME: write the marker last, to signal completed transaction
    eeprom_update_byte((uint8_t *)offset, EEP_MARKER);
    offset ++;
    // user data
    for(uint8_t i=0; i<EEPROM_WL_BYTES; i++, offset++) {
        eeprom_update_byte((uint8_t *)(offset), eeprom_wl[i]);
    }
    sei();
}
#endif


#endif
