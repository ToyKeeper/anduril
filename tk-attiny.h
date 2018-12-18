#ifndef TK_ATTINY_H
#define TK_ATTINY_H
/*
 * Attiny portability header.
 * This helps abstract away the differences between various attiny MCUs.
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
 *
 */

// Choose your MCU here, or in the main .c file, or in the build script
//#define ATTINY 13
//#define ATTINY 25

/******************** hardware-specific values **************************/
#if (ATTINY == 13)
    #define F_CPU 4800000UL
    #define EEPSIZE 64
    #define V_REF REFS0
    #define BOGOMIPS 950
    #define ADMUX_VCC 0b00001100
    #define DELAY_ZERO_TIME 252
#elif (ATTINY == 25)
    // TODO: Use 6.4 MHz instead of 8 MHz?
    #define F_CPU 8000000UL
    #define EEPSIZE 128
    #define V_REF REFS1
    #define BOGOMIPS (F_CPU/4000)
    #define ADMUX_VCC 0b00001100
    #define ADMUX_THERM 0b10001111
    #define DELAY_ZERO_TIME 1020
#elif (ATTINY == 85)
    // TODO: Use 6.4 MHz instead of 8 MHz?
    #define F_CPU 8000000UL
    #define EEPSIZE 512
    #define V_REF REFS1
    #define BOGOMIPS (F_CPU/4000)
    // (1 << V_REF) | (0 << ADLAR) | (VCC_CHANNEL)
    #define ADMUX_VCC 0b00001100
    // (1 << V_REF) | (0 << ADLAR) | (THERM_CHANNEL)
    #define ADMUX_THERM 0b10001111
    #define DELAY_ZERO_TIME 1020
#else
    #error Hey, you need to define ATTINY.
#endif


#include <avr/interrupt.h>

/******************** I/O pin and register layout ************************/
#ifdef HWDEFFILE
#include "tk.h"
#include incfile(HWDEFFILE)
#endif

#if 0  // placeholder

#elif defined(NANJG_LAYOUT)
#include "hwdef-nanjg.h"

#elif defined(FET_7135_LAYOUT)
#include "hwdef-FET_7135.h"

#elif defined(TRIPLEDOWN_LAYOUT)
#include "hwdef-Tripledown.h"

#elif defined(FERRERO_ROCHER_LAYOUT)
#include "hwdef-Ferrero_Rocher.h"

#endif  // no more recognized driver types

#ifndef LAYOUT_DEFINED
#error Hey, you need to define an I/O pin layout.
#endif

#endif  // TK_ATTINY_H
