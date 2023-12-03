// arch/mcu.h: Attiny portability header.
// Copyright (C) 2014-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// This helps abstract away the differences between various attiny MCUs.

#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


// for consistency, ROM_SIZE + EEPROM_SIZE
#define ROM_SIZE  PROGMEM_SIZE

#include "fsm/tk.h"

#define MCU_H arch/MCUNAME.h
#define MCU_C arch/MCUNAME.c
#include incfile(MCU_H)

