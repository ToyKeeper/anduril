#ifndef TK_VOLTAGE_H
#define TK_VOLTAGE_H
/*
 * Voltage / battcheck functions.
 *
 * Copyright (C) 2015 Selene ToyKeeper
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
 *
 */

#include "tk-attiny.h"
#include "tk-calibration.h"

#ifdef VOLTAGE_MON
inline void ADC_on() {
    // disable digital input on ADC pin to reduce power consumption
    DIDR0 |= (1 << ADC_DIDR);
    // 1.1v reference, left-adjust, ADC1/PB2
    ADMUX  = (1 << V_REF) | (1 << ADLAR) | ADC_CHANNEL;
    // enable, start, prescale
    ADCSRA = (1 << ADEN ) | (1 << ADSC ) | ADC_PRSCL;
}

uint8_t get_voltage() {
    // Start conversion
    ADCSRA |= (1 << ADSC);
    // Wait for completion
    while (ADCSRA & (1 << ADSC));
    // Send back the result
    return ADCH;
}
#else
inline void ADC_off() {
    ADCSRA &= ~(1<<7); //ADC off
}
#endif

#ifdef USE_BATTCHECK
PROGMEM const uint8_t voltage_blinks[] = {
              // 0 blinks for less than 1%
    ADC_0,    // 1 blink  for 1%-25%
    ADC_25,   // 2 blinks for 25%-50%
    ADC_50,   // 3 blinks for 50%-75%
    ADC_75,   // 4 blinks for 75%-100%
    ADC_100,  // 5 blinks for >100%
    255,      // Ceiling, don't remove  (6 blinks means "error")
};

inline uint8_t battcheck() {
    // Return an int, number of "blinks", for approximate battery charge
    // Uses the table above for return values
    uint8_t i, voltage;
    voltage = get_voltage();
    // figure out how many times to blink
    for (i=0;
         voltage > pgm_read_byte(voltage_blinks + i);
         i ++) {}
    return i;
}
#endif


#endif  // TK_VOLTAGE_H
