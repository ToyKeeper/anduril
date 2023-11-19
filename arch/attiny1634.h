// arch/attiny1634.h: attiny1634 support header
// Copyright (C) 2014-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// fill in missing values from Atmel's headers
#define PROGMEM_SIZE  16384
#define EEPROM_SIZE   256

////////// clock speed / delay stuff //////////

#define F_CPU  8000000UL
#define BOGOMIPS  (F_CPU/4000)
#define DELAY_ZERO_TIME  1020

///// clock dividers
inline void clock_prescale_set(uint8_t n);

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


////////// ADC voltage / temperature //////////

#define V_REF REFS1
//#define VOLTAGE_ADC_DIDR DIDR0  // set this in hwdef

// DS table 19-3, 19-4, 1.1V ref / VCC
#define ADMUX_VCC 0b00001101
// (1 << V_REF) | (THERM_CHANNEL)
// DS table 19-3, 19-4, internal sensor / 1.1V ref
#define ADMUX_THERM 0b10001110




#define hwdef_set_admux_therm  mcu_set_admux_therm
inline void mcu_set_admux_therm();

#define hwdef_set_admux_voltage mcu_set_admux_voltage
inline void mcu_set_admux_voltage();

inline void mcu_adc_sleep_mode();

inline void mcu_adc_start_measurement();

inline void mcu_adc_on();

inline void mcu_adc_off();

// NOP because interrupt flag clears itself
#define mcu_adc_vect_clear()

inline uint16_t mcu_adc_result();

inline uint8_t mcu_adc_lsb();


////////// WDT //////////

inline void mcu_wdt_active();

inline void mcu_wdt_standby();

inline void mcu_wdt_stop();

// NOP because interrupt flag clears itself
#define mcu_wdt_vect_clear()


////////// PCINT - pin change interrupt (e-switch) //////////

// set these in hwdef
//#define SWITCH_PORT  PINA
//#define SWITCH_VECT  PCINT0_vect

// NOP because interrupt flag clears itself
#define mcu_switch_vect_clear()

inline void mcu_pcint_on();

inline void mcu_pcint_off();


////////// misc //////////

void reboot();

inline void prevent_reboot_loop();

