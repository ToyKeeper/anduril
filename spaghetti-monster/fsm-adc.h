/*
 * fsm-adc.h: ADC (voltage, temperature) functions for SpaghettiMonster.
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
 */

#ifndef FSM_ADC_H
#define FSM_ADC_H


#ifdef USE_LVP
// default 5 seconds between low-voltage warning events
#ifndef VOLTAGE_WARNING_SECONDS
#define VOLTAGE_WARNING_SECONDS 5
#endif
// low-battery threshold in volts * 10
#ifndef VOLTAGE_LOW
#define VOLTAGE_LOW 29
#endif
// MCU sees voltage 0.X volts lower than actual, add X/2 to readings
#ifndef VOLTAGE_FUDGE_FACTOR
#ifdef USE_VOLTAGE_DIVIDER
#define VOLTAGE_FUDGE_FACTOR 0
#else
#define VOLTAGE_FUDGE_FACTOR 5
#endif
#endif

volatile uint8_t irq_adc = 0;  // ADC interrupt happened?
uint16_t adc_sample_count = 0;  // skip the first sample; it's junk
uint8_t adc_channel = 0;  // 0=voltage, 1=temperature
uint16_t adc_raw[2];  // last ADC measurements (0=voltage, 1=temperature)
uint16_t adc_smooth[2];  // lowpassed ADC measurements (0=voltage, 1=temperature)
// ADC code is split into two parts:
// - ISR: runs immediately at each interrupt, does the bare minimum because time is critical here
// - deferred: the bulk of the logic runs later when time isn't so critical
uint8_t adc_deferred_enable = 0;  // stop waiting and run the deferred code
void adc_deferred();  // do the actual ADC-related calculations

static inline void ADC_voltage_handler();
volatile uint8_t voltage = 0;
void low_voltage();

#ifdef USE_BATTCHECK
void battcheck();
#ifdef BATTCHECK_VpT
#define USE_BLINK_NUM
#endif
#if defined(BATTCHECK_8bars) || defined(BATTCHECK_6bars) || defined(BATTCHECK_4bars)
#define USE_BLINK_DIGIT
#endif
#endif
#endif  // ifdef USE_LVP


#ifdef USE_THERMAL_REGULATION
// default 1 seconds between thermal regulation events
#ifndef THERMAL_WARNING_SECONDS
#define THERMAL_WARNING_SECONDS 1
#endif
// try to keep temperature below 45 C
#ifndef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 45
#endif
// don't allow user to set ceiling above 70 C
#ifndef MAX_THERM_CEIL
#define MAX_THERM_CEIL 70
#endif
// Local per-MCU calibration value
#ifndef THERM_CAL_OFFSET
#define THERM_CAL_OFFSET 0
#endif
// temperature now, in C (ish) * 2  (14.1 fixed-point)
volatile int16_t temperature;
// temperature in a few seconds, in C (ish) * 2  (14.1 fixed-point)
volatile int16_t projected_temperature;  // Fight the future!
uint8_t therm_ceil = DEFAULT_THERM_CEIL;
int8_t therm_cal_offset = 0;
//void low_temperature();
//void high_temperature();
volatile uint8_t reset_thermal_history = 1;
static inline void ADC_temperature_handler();
#endif  // ifdef USE_THERMAL_REGULATION


inline void ADC_on();
inline void ADC_off();
inline void ADC_start_measurement();


#endif
