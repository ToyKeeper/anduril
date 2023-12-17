// arch/avr32dd20.h: avr32dd20 support header
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

////////// clock speed / delay stuff //////////

#define F_CPU  12000000UL
#define BOGOMIPS  (F_CPU/3800)
#define DELAY_ZERO_TIME  1020

inline void mcu_clock_speed();

///// clock dividers
// this should work, but needs further validation
inline void clock_prescale_set(uint8_t n);

// TODO: allow hwdef to define a base clock speed,
//       and adjust these values accordingly
typedef enum
{
    // Actual clock is 12 MHz
    clock_div_1 =   (0), // 12 MHz
    clock_div_2 =   (CLKCTRL_PDIV_2X_gc  | CLKCTRL_PEN_bm), // 6 MHz
    clock_div_4 =   (CLKCTRL_PDIV_4X_gc  | CLKCTRL_PEN_bm), // 3 MHz
    clock_div_8 =   (CLKCTRL_PDIV_8X_gc  | CLKCTRL_PEN_bm), // 1.5 MHz
    clock_div_16 =  (CLKCTRL_PDIV_16X_gc | CLKCTRL_PEN_bm), // 0.75 MHz
    clock_div_32 =  (CLKCTRL_PDIV_32X_gc | CLKCTRL_PEN_bm), // 375 kHz
    clock_div_64 =  (CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm), // 187.5 kHz
} clock_div_t;


////////// DAC controls //////////

// main LED outputs
#define DAC_LVL   DAC0_DATA    // 0 to 1023, for 0V to Vref
#define DAC_VREF  VREF_DAC0REF // 1.024V, 2.048V, 4.096V, or 2.5V

// Vref values (suitable for DAC and ADC0)
#define V10   VREF_REFSEL_1V024_gc
#define V20   VREF_REFSEL_2V048_gc
#define V25   VREF_REFSEL_2V500_gc
#define V40   VREF_REFSEL_4V096_gc


////////// ADC voltage / temperature //////////

#define hwdef_set_admux_therm  mcu_set_admux_therm
inline void mcu_set_admux_therm();

#define hwdef_set_admux_voltage mcu_set_admux_voltage
inline void mcu_set_admux_voltage();

inline void mcu_adc_sleep_mode();

inline void mcu_adc_start_measurement();

//#define mcu_adc_on  hwdef_set_admux_voltage
//void mcu_adc_on();

inline void mcu_adc_off();

#define ADC_vect  ADC0_RESRDY_vect
inline void mcu_adc_vect_clear();

// both readings are left-aligned
inline uint16_t mcu_adc_result();

// read ADC differently for temperature and voltage
//#define MCU_ADC_RESULT_PER_TYPE
//inline uint16_t mcu_adc_result_temp();
//inline uint16_t mcu_adc_result_volts();

// return Volts * 40, range 0 to 6.375V
#define voltage_raw2cooked  mcu_vdd_raw2cooked
inline uint8_t mcu_vdd_raw2cooked(uint16_t measurement);

// return (temp in Kelvin << 6)
#define temp_raw2cooked  mcu_temp_raw2cooked
inline uint16_t mcu_temp_raw2cooked(uint16_t measurement);

inline uint8_t mcu_adc_lsb();


////////// WDT //////////

inline void mcu_wdt_active();

inline void mcu_wdt_standby();

inline void mcu_wdt_stop();

#define WDT_vect  RTC_PIT_vect
inline void mcu_wdt_vect_clear();


////////// PCINT - pin change interrupt (e-switch) //////////

// set these in hwdef
//#define SWITCH_PORT     VPORTD.IN
//#define SWITCH_VECT     PORTD_PORT_vect

inline void mcu_switch_vect_clear();

inline void mcu_pcint_on();

inline void mcu_pcint_off();


////////// misc //////////

void reboot();

inline void prevent_reboot_loop();

