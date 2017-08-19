/*
 * fsm-adc.c: ADC (voltage, temperature) functions for SpaghettiMonster.
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

#ifndef FSM_ADC_C
#define FSM_ADC_C

inline void ADC_on()
{
    // read voltage on VCC by default
    // disable digital input on VCC pin to reduce power consumption
    //DIDR0 |= (1 << ADC_DIDR);  // FIXME: unsure how to handle for VCC pin
    // VCC / 1.1V reference
    ADMUX = ADMUX_VCC;
    // enable, start, prescale
    ADCSRA = (1 << ADEN) | (1 << ADSC) | ADC_PRSCL;
}

inline void ADC_off() {
    ADCSRA &= ~(1<<ADEN); //ADC off
}

// TODO: is this better done in main() or WDT()?
ISR(ADC_vect) {
    static uint8_t adc_step = 0;
    #ifdef USE_LVP
    #ifdef USE_LVP_AVG
    #define NUM_VOLTAGE_VALUES 4
    static int16_t voltage_values[NUM_VOLTAGE_VALUES];
    #endif
    static uint8_t lvp_timer = 0;
    static uint8_t lvp_lowpass = 0;
    #define LVP_TIMER_START 50  // ticks between LVP warnings
    #define LVP_LOWPASS_STRENGTH 4
    #endif

    #ifdef USE_THERMAL_REGULATION
    #define NUM_THERMAL_VALUES 4
    #define ADC_STEPS 4
    static int16_t temperature_values[NUM_THERMAL_VALUES];
    #else
    #define ADC_STEPS 2
    #endif

    uint16_t measurement = ADC;  // latest 10-bit ADC reading

    adc_step = (adc_step + 1) & (ADC_STEPS-1);

    #ifdef USE_LVP
    // voltage
    if (adc_step == 1) {
        #ifdef USE_LVP_AVG
        // prime on first execution
        if (voltage == 0) {
            for(uint8_t i=0; i<NUM_VOLTAGE_VALUES; i++)
                voltage_values[i] = measurement;
            voltage = 42;  // Life, the Universe, and Everything (*)
        } else {
            uint16_t total = 0;
            uint8_t i;
            for(i=0; i<NUM_VOLTAGE_VALUES-1; i++) {
                voltage_values[i] = voltage_values[i+1];
                total += voltage_values[i];
            }
            voltage_values[i] = measurement;
            total += measurement;
            total = total >> 2;

            voltage = (uint16_t)(1.1*1024*10)/total + VOLTAGE_FUDGE_FACTOR;
        }
        #else  // no USE_LVP_AVG
        // calculate actual voltage: volts * 10
        // ADC = 1.1 * 1024 / volts
        // volts = 1.1 * 1024 / ADC
        voltage = (uint16_t)(1.1*1024*10)/measurement + VOLTAGE_FUDGE_FACTOR;
        #endif
        // if low, callback EV_voltage_low / EV_voltage_critical
        //         (but only if it has been more than N ticks since last call)
        if (lvp_timer) {
            lvp_timer --;
        } else {  // it has been long enough since the last warning
            if (voltage < VOLTAGE_LOW) {
                if (lvp_lowpass < LVP_LOWPASS_STRENGTH) {
                    lvp_lowpass ++;
                } else {
                    // try to send out a warning
                    //uint8_t err = emit(EV_voltage_low, 0);
                    //uint8_t err = emit_now(EV_voltage_low, 0);
                    emit(EV_voltage_low, 0);
                    //if (!err) {
                        // on successful warning, reset counters
                        lvp_timer = LVP_TIMER_START;
                        lvp_lowpass = 0;
                    //}
                }
            } else {
                // voltage not low?  reset count
                lvp_lowpass = 0;
            }
        }
    }
    #endif  // ifdef USE_LVP

    // TODO: temperature

    // start another measurement for next time
    #ifdef USE_THERMAL_REGULATION
        #ifdef USE_LVP
        if (adc_step < 2) ADMUX = ADMUX_VCC;
        else ADMUX = ADMUX_THERM;
        #else
        ADMUX = ADMUX_THERM;
        #endif
    #else
        #ifdef USE_LVP
        ADMUX = ADMUX_VCC;
        #endif
    #endif
}

#endif
