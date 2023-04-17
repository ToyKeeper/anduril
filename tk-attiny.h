// tk-attiny.h: Attiny portability header.
// Copyright (C) 2014-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// This helps abstract away the differences between various attiny MCUs.

// Choose your MCU here, or in the main .c file, or in the build script
//#define ATTINY 13
//#define ATTINY 25

/******************** hardware-specific values **************************/
#if (ATTINY == 13)
    #define F_CPU 4800000UL
    //#define EEPSIZE 64
    #define V_REF REFS0
    #define BOGOMIPS 950
    #define ADMUX_VCC 0b00001100
    #define DELAY_ZERO_TIME 252
    #define SWITCH_PORT  PINB  // PINA or PINB or PINC
    #define VOLTAGE_ADC_DIDR DIDR0  // this MCU only has one DIDR
#elif (ATTINY == 25)
    // TODO: Use 6.4 MHz instead of 8 MHz?
    #define F_CPU 8000000UL
    //#define EEPSIZE 128
    #define V_REF REFS1
    #define BOGOMIPS (F_CPU/4000)
    #define ADMUX_VCC 0b00001100
    #define ADMUX_THERM 0b10001111
    #define DELAY_ZERO_TIME 1020
    #define SWITCH_PORT  PINB  // PINA or PINB or PINC
    #define VOLTAGE_ADC_DIDR DIDR0  // this MCU only has one DIDR
#elif (ATTINY == 85)
    // TODO: Use 6.4 MHz instead of 8 MHz?
    #define F_CPU 8000000UL
    //#define EEPSIZE 512
    #define V_REF REFS1
    #define BOGOMIPS (F_CPU/4000)
    // (1 << V_REF) | (0 << ADLAR) | (VCC_CHANNEL)
    #define ADMUX_VCC 0b00001100
    // (1 << V_REF) | (0 << ADLAR) | (THERM_CHANNEL)
    #define ADMUX_THERM 0b10001111
    #define DELAY_ZERO_TIME 1020
    #define SWITCH_PORT  PINB  // PINA or PINB or PINC
    #define VOLTAGE_ADC_DIDR DIDR0  // this MCU only has one DIDR
#elif (ATTINY == 1634)
    #define F_CPU 8000000UL
    #define V_REF REFS1
    #define BOGOMIPS (F_CPU/4000)
    // DS table 19-3, 19-4, 1.1V ref / VCC
    #define ADMUX_VCC 0b00001101
    // (1 << V_REF) | (THERM_CHANNEL)
    // DS table 19-3, 19-4, internal sensor / 1.1V ref
    #define ADMUX_THERM 0b10001110
    #define DELAY_ZERO_TIME 1020
    //#define SWITCH_PORT  PINA  // set this in hwdef
    //#define VOLTAGE_ADC_DIDR DIDR0  // set this in hwdef
#elif (ATTINY == 412) || (ATTINY == 416) || (ATTINY == 417) || (ATTINY == 816) || (ATTINY == 817) || (ATTINY == 1616) || (ATTINY == 1617) || (ATTINY == 3216) || (ATTINY == 3217)
    #define AVRXMEGA3
    #define F_CPU 10000000UL
    #define BOGOMIPS (F_CPU/4000)
    #define EEPSIZE 128
    #define DELAY_ZERO_TIME 1020
#else
    #error Hey, you need to define ATTINY.
#endif

// auto-detect eeprom size from avr-libc headers
#ifndef EEPSIZE
#ifdef E2SIZE
#define EEPSIZE E2SIZE
#else
#define EEPSIZE (E2END+1)
#endif
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

#if (ATTINY==13)
  // no changes needed
#elif (ATTINY==25) || (ATTINY==45) || (ATTINY==85)
  // use clock_prescale_set(n) instead; it's safer
  //#define CLOCK_DIVIDER_SET(n) {CLKPR = 1<<CLKPCE; CLKPR = n;}
#elif (ATTINY==1634)
  // make it a NOP for now
  // FIXME
  //#define clock_prescale_set(x) ((void)0)
  //#define clock_prescale_set(n) {cli(); CCP = 0xD8; CLKPR = n; sei();}
  //#define clock_prescale_set(n) {cli(); CCP = 0xD8; CLKPR = n; sei();}
  inline void clock_prescale_set(uint8_t n) {cli(); CCP = 0xD8; CLKPR = n; sei();}
  typedef enum
  {
      clock_div_1 = 0,
      clock_div_2 = 1,
      clock_div_4 = 2,
      clock_div_8 = 3,
      clock_div_16 = 4,
      clock_div_32 = 5,
      clock_div_64 = 6,
      clock_div_128 = 7,
      clock_div_256 = 8
  } clock_div_t;

#elif defined(AVRXMEGA3)  // ATTINY816, 817, etc
    // this should work, but needs further validation
    inline void clock_prescale_set(uint8_t n) {
        cli();         
        CCP = CCP_IOREG_gc; // temporarily disable clock change protection
        CLKCTRL.MCLKCTRLB = n; // Set the prescaler
        while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {} // wait for clock change to finish
        sei();
    }
    typedef enum
    {
      // Actual clock is 20 MHz, but assume that 10 MHz is the top speed and work from there
      // TODO: measure PWM speed and power use at 1.25/2.5/5/10 MHz, to determine which speeds are optimal 
      clock_div_1 =   (CLKCTRL_PDIV_2X_gc  | CLKCTRL_PEN_bm), // 10 MHz
      clock_div_2 =   (CLKCTRL_PDIV_4X_gc  | CLKCTRL_PEN_bm), // 5 MHz
      clock_div_4 =   (CLKCTRL_PDIV_8X_gc  | CLKCTRL_PEN_bm), // 2.5 MHz
      clock_div_8 =   (CLKCTRL_PDIV_16X_gc | CLKCTRL_PEN_bm), // 1.25 MHz
      clock_div_16 =  (CLKCTRL_PDIV_32X_gc | CLKCTRL_PEN_bm), // 625 kHz
      clock_div_32 =  (CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm), // 312 kHz, max without changing to the 32 kHz ULP
      clock_div_64 =  (CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm), // 312 kHz 
      clock_div_128 = (CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm), // 312 kHz 
      clock_div_256 = (CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm)  // 312 kHz 
    } clock_div_t;
#else
#error Unable to define MCU macros.
#endif

