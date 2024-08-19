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

inline void mcu_clock_speed();

///// clock dividers
inline void clock_prescale_set(uint8_t n);

// TODO? allow hwdef to define a base clock speed,
//       and adjust these values accordingly
typedef enum
{
    // datasheet 6.5.2, CLKPR - Clock Prescale Register
    clock_div_1   = 0,  // 8 MHz
    clock_div_2   = 1,  // 4 MHz
    clock_div_4   = 2,  // 2 MHz
    clock_div_8   = 3,  // 1 MHz
    clock_div_16  = 4,  // 500 kHz
    clock_div_32  = 5,  // 250 kHz
    clock_div_64  = 6,  // 125 kHz
    clock_div_128 = 7,  // 62.5 kHz
    clock_div_256 = 8,  // 31.75 kHz
} clock_div_t;


////////// ADC voltage / temperature //////////

#define V_REF REFS1
//#define VOLTAGE_ADC_DIDR DIDR0  // set this in hwdef

// DS table 19-3, 19-4, 1.1V ref / VCC
#define ADMUX_VCC 0b00001101
// (1 << V_REF) | (THERM_CHANNEL)
// DS table 19-3, 19-4, internal sensor / 1.1V ref
// [refs1, refs0, refen, adc0en, mux3, mux2, mux1, mux0]
// refs=0b10 : internal 1.1V ref
// mux=0b1110 : internal temperature sensor
#define ADMUX_THERM 0b10001110

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

