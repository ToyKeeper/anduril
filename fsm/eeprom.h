// fsm-eeprom.h: EEPROM API for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

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

