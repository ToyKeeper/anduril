// fsm-adc.h: ADC (voltage, temperature) functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// voltage is 0.00V to 5.10V in 0.02V steps, from 0 to 255
// so one deci-Volt is 5 steps
#define dV 5

#if defined(USE_LVP) || defined(USE_THERMAL_REGULATION)
// use raw value instead of lowpassed value for the next N measurements
// (2 = 1 for voltage + 1 for temperature)
volatile uint8_t adc_reset = 2;
#endif

#ifdef USE_LVP
// default 5 seconds between low-voltage warning events
#ifndef VOLTAGE_WARNING_SECONDS
#define VOLTAGE_WARNING_SECONDS 5
#endif
// low-battery threshold in volts * 50
#ifndef VOLTAGE_LOW
#define VOLTAGE_LOW (29*dV)
#endif
// battery is low but not critical
#ifndef VOLTAGE_RED
#define VOLTAGE_RED (33*dV)
#endif
// MCU sees voltage 0.X volts lower than actual, add X/2 to readings
#ifndef VOLTAGE_FUDGE_FACTOR
#ifdef USE_VOLTAGE_DIVIDER
#define VOLTAGE_FUDGE_FACTOR 0
#else
#define VOLTAGE_FUDGE_FACTOR 5
#endif
#endif


void adc_voltage_mode();

#ifdef TICK_DURING_STANDBY
volatile uint8_t adc_active_now = 0;  // sleep LVP needs a different sleep mode
#endif
volatile uint8_t irq_adc = 0;  // ADC interrupt happened?
uint8_t adc_sample_count = 0;  // skip the first sample; it's junk
uint8_t adc_channel = 0;  // 0=voltage, 1=temperature
uint16_t adc_raw[2];  // last ADC measurements (0=voltage, 1=temperature)
uint16_t adc_smooth[2];  // lowpassed ADC measurements (0=voltage, 1=temperature)
// ADC code is split into two parts:
// - ISR: runs immediately at each interrupt, does the bare minimum because time is critical here
// - deferred: the bulk of the logic runs later when time isn't so critical
uint8_t adc_deferred_enable = 0;  // stop waiting and run the deferred code
void adc_deferred();  // do the actual ADC-related calculations

static void ADC_voltage_handler();
uint8_t voltage = 0;
#ifdef USE_VOLTAGE_CORRECTION
    #ifdef USE_CFG
        #define VOLT_CORR cfg.voltage_correction
    #else
        // same 0.05V units as fudge factor,
        // but 7 is neutral, and the expected range is from 1 to 13
        uint8_t voltage_correction = 7;
        #define VOLT_CORR voltage_correction
    #endif
#endif
#ifdef USE_LVP
void low_voltage();
#endif

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
// temperature now, in C (ish)
int16_t temperature;
#ifdef USE_CFG
    #define TH_CEIL cfg.therm_ceil
    #define TH_CAL cfg.therm_cal_offset
#else
    #define TH_CEIL therm_ceil
    #define TH_CAL therm_cal_offset
    uint8_t therm_ceil = DEFAULT_THERM_CEIL;
    int8_t therm_cal_offset = 0;
#endif
static void ADC_temperature_handler();
#endif  // ifdef USE_THERMAL_REGULATION


//inline void ADC_on();
#define ADC_on   adc_voltage_mode
//inline void ADC_off();
#define ADC_off  mcu_adc_off
//inline void ADC_start_measurement();
#define ADC_start_measurement  mcu_adc_start_measurement

// needs a special sleep mode to get accurate measurements quickly 
// ... full power-down ends up using more power overall, and causes 
// some weird issues when the MCU doesn't stay awake enough cycles 
// to complete a reading
//inline void adc_sleep_mode();
#define adc_sleep_mode  mcu_adc_sleep_mode

