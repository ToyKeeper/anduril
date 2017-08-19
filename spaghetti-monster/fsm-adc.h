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
// volts * 10
#define VOLTAGE_LOW 30
// MCU sees voltage 0.X volts lower than actual, add X to readings
#define VOLTAGE_FUDGE_FACTOR 2
volatile uint8_t voltage;
void low_voltage();
#endif
#ifdef USE_THERMAL_REGULATION
volatile int16_t temperature;
void low_temperature();
void high_temperature();
#endif

inline void ADC_on();
inline void ADC_off();

#endif
