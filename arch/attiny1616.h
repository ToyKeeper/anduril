// arch/attiny1616.h: attiny1616 support header
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// FIXME: remove this
#define AVRXMEGA3

////////// clock speed / delay stuff //////////

#define F_CPU  10000000UL
#define BOGOMIPS  (F_CPU/4350)
#define DELAY_ZERO_TIME  1020

inline void mcu_clock_speed();

///// clock dividers
// this should work, but needs further validation
inline void clock_prescale_set(uint8_t n);

// TODO: allow hwdef to define a base clock speed,
//       and adjust these values accordingly
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


////////// DAC controls //////////

#define DAC_LVL   DAC0.DATA    // 0 to 255, for 0V to Vref
#define DAC_VREF  VREF.CTRLA   // 0.55V, 1.1V, 1.5V, 2.5V, or 4.3V

// set only the relevant bits of the Vref register
#define mcu_set_dac_vref(x) \
    VREF.CTRLA = x | (VREF.CTRLA & (~VREF_DAC0REFSEL_gm));

// Vref values
// (for the DAC bits, not the ADC bits)
#define V05   V055
#define V055  VREF_DAC0REFSEL_0V55_gc
#define V11   VREF_DAC0REFSEL_1V1_gc
#define V25   VREF_DAC0REFSEL_2V5_gc
#define V43   VREF_DAC0REFSEL_4V34_gc
#define V15   VREF_DAC0REFSEL_1V5_gc

////////// ADC voltage / temperature //////////

// set only the relevant bits of the Vref register
#define mcu_set_adc0_vref(x) \
    VREF.CTRLA = x | (VREF.CTRLA & (~VREF_ADC0REFSEL_gm));

#define hwdef_set_admux_therm  mcu_set_admux_therm
inline void mcu_set_admux_therm();

#define hwdef_set_admux_voltage mcu_set_admux_voltage
inline void mcu_set_admux_voltage();

inline void mcu_adc_sleep_mode();

inline void mcu_adc_start_measurement();

//inline void mcu_adc_on();

inline void mcu_adc_off();

#define ADC_vect  ADC0_RESRDY_vect
inline void mcu_adc_vect_clear();

//// both readings are left-aligned
//inline uint16_t mcu_adc_result();

// read ADC differently for temperature and voltage
#define MCU_ADC_RESULT_PER_TYPE
inline uint16_t mcu_adc_result_temp();
inline uint16_t mcu_adc_result_volts();

// return Volts * 40, range 0 to 6.375V
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

// *** Note for the AVRXMEGA3 (1-Series, eg 816 and 817), the WDT 
// is not used for time-based interrupts.  A new peripheral, the 
// Periodic Interrupt Timer ("PIT") is used for this purpose.

#define WDT_vect  RTC_PIT_vect
inline void mcu_wdt_vect_clear();


////////// PCINT - pin change interrupt (e-switch) //////////

// set these in hwdef
//#define SWITCH_PORT  PINA
//#define SWITCH_VECT  PCINT0_vect

inline void mcu_switch_vect_clear();

inline void mcu_pcint_on();

inline void mcu_pcint_off();


////////// misc //////////

void reboot();

inline void prevent_reboot_loop();

