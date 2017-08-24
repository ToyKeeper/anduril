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

// Each full cycle runs 7.8X per second with just voltage enabled,
// or 3.9X per second with voltage and temperature.
#if defined(USE_LVP) && defined(USE_THERMAL_REGULATION)
#define ADC_CYCLES_PER_SECOND 4
#else
#define ADC_CYCLES_PER_SECOND 8
#endif
// TODO: is this better done in main() or WDT()?
ISR(ADC_vect) {
    static uint8_t adc_step = 0;

    // LVP declarations
    #ifdef USE_LVP
    #ifdef USE_LVP_AVG
    #define NUM_VOLTAGE_VALUES 4
    static int16_t voltage_values[NUM_VOLTAGE_VALUES];
    #endif
    static uint8_t lvp_timer = 0;
    static uint8_t lvp_lowpass = 0;
    #define LVP_TIMER_START (VOLTAGE_WARNING_SECONDS*ADC_CYCLES_PER_SECOND)  // N seconds between LVP warnings
    #define LVP_LOWPASS_STRENGTH ADC_CYCLES_PER_SECOND  // lowpass for one second
    #endif

    // thermal declarations
    #ifdef USE_THERMAL_REGULATION
    #define NUM_THERMAL_VALUES 4
    #define NUM_THERMAL_VALUES_HISTORY 16
    #define ADC_STEPS 4
    static uint8_t first_temp_reading = 1;
    static int16_t temperature_values[NUM_THERMAL_VALUES];  // last few readings in C
    static int16_t temperature_history[NUM_THERMAL_VALUES_HISTORY];  // 13.2 fixed-point
    static uint8_t temperature_timer = 0;
    static uint8_t overheat_lowpass = 0;
    static uint8_t underheat_lowpass = 0;
    #define TEMPERATURE_TIMER_START (THERMAL_WARNING_SECONDS*ADC_CYCLES_PER_SECOND)  // N seconds between thermal regulation events
    #define OVERHEAT_LOWPASS_STRENGTH ADC_CYCLES_PER_SECOND  // lowpass for one second
    #define UNDERHEAT_LOWPASS_STRENGTH ADC_CYCLES_PER_SECOND  // lowpass for one second
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


    #ifdef USE_THERMAL_REGULATION
    // temperature
    else if (adc_step == 3) {
        // Convert ADC units to Celsius (ish)
        int16_t temp = measurement - 275 + THERM_CAL_OFFSET;

        // prime on first execution
        if (first_temp_reading) {
            first_temp_reading = 0;
            for(uint8_t i=0; i<NUM_THERMAL_VALUES; i++)
                temperature_values[i] = temp;
            for(uint8_t i=0; i<NUM_THERMAL_VALUES_HISTORY; i++)
                temperature_history[i] = temp<<2;
            temperature = temp;
        } else {  // update our current temperature estimate
            uint8_t i;
            int16_t total=0;

            // rotate array
            for(i=0; i<NUM_THERMAL_VALUES-1; i++) {
                temperature_values[i] = temperature_values[i+1];
                total += temperature_values[i];
            }
            temperature_values[i] = temp;
            total += temp;

            // Divide back to original range:
            //temperature = total >> 2;
            // More precise method: use noise as extra precision
            // (values are now basically fixed-point, signed 13.2)
            temperature = total;
        }

        // guess what the temperature will be in a few seconds
        {
            uint8_t i;
            int16_t diff;

            // algorithm tweaking; not really intended to be modified
            // how far ahead should we predict?
            #define THERM_PREDICTION_STRENGTH 4
            // how proportional should the adjustments be?
            #define THERM_DIFF_ATTENUATION 4
            // acceptable temperature window size in C
            #define THERM_WINDOW_SIZE 8
            // highest temperature allowed
            // (convert configured value to 13.2 fixed-point)
            #define THERM_CEIL (therm_ceil<<2)
            // bottom of target temperature window (13.2 fixed-point)
            #define THERM_FLOOR (THERM_CEIL - (THERM_WINDOW_SIZE<<2))

            // rotate measurements and add a new one
            for (i=0; i<NUM_THERMAL_VALUES_HISTORY-1; i++) {
                temperature_history[i] = temperature_history[i+1];
            }
            temperature_history[NUM_THERMAL_VALUES_HISTORY-1] = temperature;

            // guess what the temp will be several seconds in the future
            diff = temperature_history[NUM_THERMAL_VALUES_HISTORY-1] - temperature_history[0];
            projected_temperature = temperature_history[NUM_THERMAL_VALUES_HISTORY-1] + (diff<<THERM_PREDICTION_STRENGTH);

        }

        // cancel counters if necessary
        if (projected_temperature > THERM_FLOOR) {
            underheat_lowpass = 0;  // we're definitely not too cold
        } else if (projected_temperature < THERM_CEIL) {
            overheat_lowpass = 0;  // we're definitely not too hot
        }

        if (temperature_timer) {
            temperature_timer --;
        } else {  // it has been long enough since the last warning

            // Too hot?
            if (projected_temperature > THERM_CEIL) {
                if (overheat_lowpass < OVERHEAT_LOWPASS_STRENGTH) {
                    overheat_lowpass ++;
                } else {
                    // how far above the ceiling?
                    int16_t howmuch = (projected_temperature - THERM_CEIL) >> THERM_DIFF_ATTENUATION;
                    if (howmuch < 1) howmuch = 1;
                    // try to send out a warning
                    emit(EV_temperature_high, howmuch);
                    // reset counters
                    temperature_timer = TEMPERATURE_TIMER_START;
                    overheat_lowpass = 0;
                }
            }

            // Too cold?
            else if (projected_temperature < THERM_FLOOR) {
                if (underheat_lowpass < UNDERHEAT_LOWPASS_STRENGTH) {
                    underheat_lowpass ++;
                } else {
                    // how far below the floor?
                    int16_t howmuch = (THERM_FLOOR - projected_temperature) >> THERM_DIFF_ATTENUATION;
                    if (howmuch < 1) howmuch = 1;
                    // try to send out a warning (unless voltage is low)
                    // (LVP and underheat warnings fight each other)
                    if (voltage > VOLTAGE_LOW)
                        emit(EV_temperature_low, howmuch);
                    // reset counters
                    temperature_timer = TEMPERATURE_TIMER_START;
                    underheat_lowpass = 0;
                }
            }
        }
    }
    #endif  // ifdef USE_THERMAL_REGULATION


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
