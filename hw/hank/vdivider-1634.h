// attiny1634 voltage divider common defs
// Copyright (C) 2020-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define USE_VOLTAGE_DIVIDER  // use a dedicated pin, not VCC, because VCC input is flattened
#define VOLTAGE_PIN        PB1  // Pin 18 / PB1 / ADC6
// pin to ADC mappings are in DS table 19-4
#define VOLTAGE_ADC      ADC6D  // digital input disable pin for PB1
// DIDR0/DIDR1 mappings are in DS section 19.13.5, 19.13.6
#define VOLTAGE_ADC_DIDR DIDR1  // DIDR channel for ADC6D
// DS tables 19-3, 19-4
// Bit   7     6     5      4     3    2    1    0
//     REFS1 REFS0 REFEN ADC0EN MUX3 MUX2 MUX1 MUX0
// MUX[3:0] = 0, 1, 1, 0 for ADC6 / PB1
// divided by ...
// REFS[1:0] = 1, 0 for internal 1.1V reference
// other bits reserved
#define ADMUX_VOLTAGE_DIVIDER  0b10000110
#undef voltage_raw2cooked
#define voltage_raw2cooked  mcu_vdivider_raw2cooked

#define ADC_PRSCL   0x07    // clk/128

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
//   (voltage - D1) * (R2/(R2+R1) * 1024 / 1.1)
// D1, R1, R2 = 0, 330, 100
#ifndef ADC_44
//#define ADC_44 (4*981)  // raw value at 4.40V
#define ADC_44 (4*967)  // manually tweaked so 4.16V will blink out 4.2
#endif
#ifndef ADC_22
//#define ADC_22 (4*489)  // raw value at 2.20V
#define ADC_22 (4*482)  // manually tweaked so 4.16V will blink out 4.2
#endif


