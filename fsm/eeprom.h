// fsm-eeprom.h: EEPROM API for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <avr/eeprom.h>

// set this higher to enable normal eeprom functions
// TODO: rename to EEPROM_BYTES_NEEDED or similar, to make purpose clearer
#ifndef EEPROM_BYTES
#define EEPROM_BYTES 0
#endif

// set this higher to enable wear-levelled eeprom functions
// TODO: rename to EEPROM_WL_BYTES_NEEDED or similar, to make purpose clearer
#ifndef EEPROM_WL_BYTES
#define EEPROM_WL_BYTES 0
#endif

#ifdef USE_EEPROM
    #ifdef USE_EEPROM_WL
        // split eeprom in half
        #define EEP_START  (EEPROM_SIZE/2)
        BUILD_ASSERT(eep_less_than_half, EEPROM_BYTES <= (EEPROM_SIZE/2));
    #else
        // use entire eeprom
        #define EEP_START  0
        BUILD_ASSERT(eep_data_fits, EEPROM_BYTES <= EEPROM_SIZE);
    #endif
    #ifdef EEPROM_OVERRIDE
        uint8_t *eeprom;
    #else
        uint8_t eeprom[EEPROM_BYTES];
    #endif
    uint8_t load_eeprom();  // returns 1 for success, 0 for no data found
    void save_eeprom();
#endif

#ifdef USE_EEPROM_WL
    #define EEP_WL_SIZE (EEPROM_SIZE/2)
    // ensure space for at least 2 slots for wear levelling
    BUILD_ASSERT(eepwl_two_slots_minimum, EEPROM_WL_BYTES <= (EEP_WL_SIZE/2));
    uint8_t eeprom_wl[EEPROM_WL_BYTES];
    uint8_t load_eeprom_wl();  // returns 1 for success, 0 for no data found
    void save_eeprom_wl();
#endif

#if EEPROM_SIZE > 256
    #define EEP_OFFSET_T uint16_t
#else
    #define EEP_OFFSET_T uint8_t
#endif

// if this marker isn't found, the eeprom is assumed to be blank
#define EEP_MARKER 0b10100101

// wait a few ms before eeprom operations, to wait for power to stabilize
// (otherwise reads or writes can get corrupt data)
// (not necessary on some hardware,
//  but enabled by default when there's space)
#if defined(LED_ENABLE_PIN) || defined(LED2_ENABLE_PIN) || (ROM_SIZE > 10000)
    #define USE_EEP_DELAY
#endif

