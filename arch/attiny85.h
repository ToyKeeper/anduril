// arch/attiny85.h: attiny85 support header
// Copyright (C) 2014-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// fill in missing values from Atmel's headers
#define PROGMEM_SIZE  8192
#define EEPROM_SIZE   512

////////// clock speed / delay stuff //////////

// TODO: Use 6.4 MHz instead of 8 MHz?
#define F_CPU  8000000UL
#define BOGOMIPS  (F_CPU/4000)
#define DELAY_ZERO_TIME  1020

///// clock dividers
// use clock_prescale_set(n) instead; it's safer
//#define CLOCK_DIVIDER_SET(n) {CLKPR = 1<<CLKPCE; CLKPR = n;}


////////// default hw_setup() //////////

#ifdef USE_GENERIC_HWDEF_SETUP
static inline void hwdef_setup();
#endif


////////// ADC voltage / temperature //////////

#define V_REF REFS1
#define VOLTAGE_ADC_DIDR DIDR0  // this MCU only has one DIDR

// (1 << V_REF) | (0 << ADLAR) | (VCC_CHANNEL)
#define ADMUX_VCC 0b00001100
// (1 << V_REF) | (0 << ADLAR) | (THERM_CHANNEL)
#define ADMUX_THERM 0b10001111

#define hwdef_set_admux_therm  mcu_set_admux_therm
inline void mcu_set_admux_therm();

#define hwdef_set_admux_voltage mcu_set_admux_voltage
inline void mcu_set_admux_voltage();

inline void mcu_adc_sleep_mode();

inline void mcu_adc_start_measurement();

inline void mcu_adc_off();

// NOP because interrupt flag clears itself
#define mcu_adc_vect_clear()

inline uint16_t mcu_adc_result();

// return Volts * 50, range 0 to 5.10V
#define voltage_raw2cooked  mcu_vdd_raw2cooked
inline uint8_t mcu_vdd_raw2cooked(uint16_t measurement);
inline uint8_t mcu_vdivider_raw2cooked(uint16_t measurement);

// return (temp in Kelvin << 6)
#define temp_raw2cooked  mcu_temp_raw2cooked
inline uint16_t mcu_temp_raw2cooked(uint16_t measurement);

inline uint8_t mcu_adc_lsb();


////////// WDT //////////

inline void mcu_wdt_active();

inline void mcu_wdt_standby();

inline void mcu_wdt_stop();

// NOP because interrupt flag clears itself
#define mcu_wdt_vect_clear()


////////// PCINT - pin change interrupt (e-switch) //////////

#define SWITCH_PORT  PINB  // PINA or PINB or PINC
#define SWITCH_VECT  PCINT0_vect

// NOP because interrupt flag clears itself
#define mcu_switch_vect_clear()

inline void mcu_pcint_on();

inline void mcu_pcint_off();


////////// misc //////////

void reboot();

inline void prevent_reboot_loop();

