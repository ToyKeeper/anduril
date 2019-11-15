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


inline void set_admux_therm() {
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85) || (ATTINY == 1634)
        ADMUX = ADMUX_THERM;
    #elif (ATTINY == 841)
        ADMUXA = ADMUXA_THERM;
        ADMUXB = ADMUXB_THERM;
    #else
        #error Unrecognized MCU type
    #endif
    adc_channel = 1;
}

inline void set_admux_voltage() {
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85) || (ATTINY == 1634)
        #ifdef USE_VOLTAGE_DIVIDER
        // 1.1V / pin7
        ADMUX = ADMUX_VOLTAGE_DIVIDER;
        #else
        // VCC / 1.1V reference
        ADMUX = ADMUX_VCC;
        #endif
    #elif (ATTINY == 841)
        #ifdef USE_VOLTAGE_DIVIDER
        ADMUXA = ADMUXA_VOLTAGE_DIVIDER;
        ADMUXB = ADMUXB_VOLTAGE_DIVIDER;
        #else
        ADMUXA = ADMUXA_VCC;
        ADMUXB = ADMUXB_VCC;
        #endif
    #else
        #error Unrecognized MCU type
    #endif
    adc_channel = 0;
}

inline void ADC_start_measurement() {
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85) || (ATTINY == 841) || (ATTINY == 1634)
        ADCSRA |= (1 << ADSC) | (1 << ADIE);
    #else
        #error unrecognized MCU type
    #endif
}

// set up ADC for reading battery voltage
inline void ADC_on()
{
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85) || (ATTINY == 1634)
        set_admux_voltage();
        #ifdef USE_VOLTAGE_DIVIDER
        // disable digital input on divider pin to reduce power consumption
        DIDR0 |= (1 << VOLTAGE_ADC_DIDR);
        #else
        // disable digital input on VCC pin to reduce power consumption
        //DIDR0 |= (1 << ADC_DIDR);  // FIXME: unsure how to handle for VCC pin
        #endif
        #if (ATTINY == 1634)
            ACSRA |= (1 << ACD);  // turn off analog comparator to save power
        #endif
        // enable, start, prescale
        ADCSRA = (1 << ADEN) | (1 << ADSC) | ADC_PRSCL;
        // end tiny25/45/85
    #elif (ATTINY == 841)
        ADCSRB = 0;  // Right adjusted, auto trigger bits cleared.
        //ADCSRA = (1 << ADEN ) | 0b011;  // ADC on, prescaler division factor 8.
        set_admux_voltage();
        // enable, start, prescale
        ADCSRA = (1 << ADEN) | (1 << ADSC) | ADC_PRSCL;
        //ADCSRA |= (1 << ADSC);  // start measuring
    #else
        #error Unrecognized MCU type
    #endif
}

inline void ADC_off() {
    ADCSRA &= ~(1<<ADEN); //ADC off
}

#ifdef USE_VOLTAGE_DIVIDER
static inline uint8_t calc_voltage_divider(uint16_t value) {
    // use 9.7 fixed-point to get sufficient precision
    uint16_t adc_per_volt = ((ADC_44<<7) - (ADC_22<<7)) / (44-22);
    // incoming value is 8.2 fixed-point, so shift it 2 bits less
    uint8_t result = ((value<<5) / adc_per_volt) + VOLTAGE_FUDGE_FACTOR;
    return result;
}
#endif

// Each full cycle runs 7.8X per second with just voltage enabled,
// or 3.9X per second with voltage and temperature.
#if defined(USE_LVP) && defined(USE_THERMAL_REGULATION)
#define ADC_CYCLES_PER_SECOND 4
#else
#define ADC_CYCLES_PER_SECOND 8
#endif

#ifdef USE_THERMAL_REGULATION
#define ADC_STEPS 2
#else
#define ADC_STEPS 1
#endif

// happens every time the ADC sampler finishes a measurement
ISR(ADC_vect) {
    #ifdef USE_PSEUDO_RAND
    // real-world entropy makes this a true random, not pseudo
    pseudo_rand_seed += ADCL;
    #endif

    if (irq_adc_stable) {  // skip first result; it's junk
        adc_values[adc_channel] = ADC;  // save this for later use
        irq_adc = 1;  // a value was saved, so trigger deferred logic
    }
    irq_adc_stable = 1;

    // start another measurement
    // (is explicit because it otherwise doesn't seem to happen during standby mode)
    ADC_start_measurement();
}

void ADC_inner() {
    irq_adc = 0;  // event handled

    // the ADC triggers repeatedly when it's on, but we only want one value
    // (so ignore everything after the first value, until it's manually reset)
    if (! adcint_enable) return;

    // disable after one iteration
    adcint_enable = 0;

    #ifdef TICK_DURING_STANDBY
        // in sleep mode, turn off after just one measurement
        // (having the ADC on raises standby power by about 250 uA)
        // (and the usual standby level is only ~20 uA)
        if (go_to_standby) ADC_off();
    #endif

    // what is being measured? 0 = battery voltage, 1 = temperature
    static uint8_t adc_step = 0;

    #ifdef USE_LVP
    if (0 == adc_step) {  // voltage
        ADC_voltage_handler();
    }
    #endif

    #ifdef USE_THERMAL_REGULATION
    else if (1 == adc_step) {  // temperature
        ADC_temperature_handler();
    }
    #endif

    #if defined(TICK_DURING_STANDBY) && defined(USE_SLEEP_LVP)
    // only measure battery voltage while asleep
    if (go_to_standby) adc_step = 0;
    else
    #endif

    adc_step = (adc_step + 1) & (ADC_STEPS-1);

    // set the correct type of measurement for next time
    #ifdef USE_THERMAL_REGULATION
        #ifdef USE_LVP
        if (0 == adc_step) set_admux_voltage();
        else set_admux_therm();
        #else
        //set_admux_therm();
        #error "USE_THERMAL_REGULATION set without USE_LVP"
        #endif
    #else
        #ifdef USE_LVP
        set_admux_voltage();
        #endif
    #endif

    irq_adc_stable = 0;  // first result is unstable
}


#ifdef USE_LVP
static inline void ADC_voltage_handler() {
    // LVP declarations
    #ifdef USE_LVP_AVG
    #define NUM_VOLTAGE_VALUES 4
    static int16_t voltage_values[NUM_VOLTAGE_VALUES];
    #endif
    static uint8_t lvp_timer = 0;
    static uint8_t lvp_lowpass = 0;
    #define LVP_TIMER_START (VOLTAGE_WARNING_SECONDS*ADC_CYCLES_PER_SECOND)  // N seconds between LVP warnings
    #define LVP_LOWPASS_STRENGTH ADC_CYCLES_PER_SECOND  // lowpass for one second

    uint16_t measurement = adc_values[0];  // latest 10-bit ADC reading

    #ifdef USE_LVP_AVG
    // prime on first execution
    if (voltage == 0) {
        for(uint8_t i=0; i<NUM_VOLTAGE_VALUES; i++)
            voltage_values[i] = measurement;
        voltage = 42;  // the answer to life, the universe, and the voltage of a full li-ion cell
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

        #ifdef USE_VOLTAGE_DIVIDER
        voltage = calc_voltage_divider(total);
        #else
        voltage = (uint16_t)(1.1*1024*10)/total + VOLTAGE_FUDGE_FACTOR;
        #endif
    }
    #else  // no USE_LVP_AVG
        #ifdef USE_VOLTAGE_DIVIDER
        voltage = calc_voltage_divider(measurement);
        #else
        // calculate actual voltage: volts * 10
        // ADC = 1.1 * 1024 / volts
        // volts = 1.1 * 1024 / ADC
        //voltage = (uint16_t)(1.1*1024*10)/measurement + VOLTAGE_FUDGE_FACTOR;
        voltage = ((uint16_t)(2*1.1*1024*10)/measurement + VOLTAGE_FUDGE_FACTOR) >> 1;
        #endif
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
#endif


#ifdef USE_THERMAL_REGULATION
static inline void ADC_temperature_handler() {
    // thermal declarations
    #ifndef THERMAL_UPDATE_SPEED
    #define THERMAL_UPDATE_SPEED 2
    #endif
    #define NUM_THERMAL_VALUES_HISTORY 8
    static uint8_t history_step = 0;  // don't update history as often
    static int16_t temperature_history[NUM_THERMAL_VALUES_HISTORY];
    static uint8_t temperature_timer = 0;
    static uint8_t overheat_lowpass = 0;
    static uint8_t underheat_lowpass = 0;
    #define TEMPERATURE_TIMER_START ((THERMAL_WARNING_SECONDS-2)*ADC_CYCLES_PER_SECOND)  // N seconds between thermal regulation events
    #define OVERHEAT_LOWPASS_STRENGTH (ADC_CYCLES_PER_SECOND*2)  // lowpass for 2 seconds
    #define UNDERHEAT_LOWPASS_STRENGTH (ADC_CYCLES_PER_SECOND*2)  // lowpass for 2 seconds

    uint16_t measurement = adc_values[1];  // latest 10-bit ADC reading

    // Convert ADC units to Celsius (ish)
    int16_t temp = measurement - 275 + THERM_CAL_OFFSET + (int16_t)therm_cal_offset;

    // prime on first execution
    if (reset_thermal_history) {
        reset_thermal_history = 0;
        temperature = temp;
        for(uint8_t i=0; i<NUM_THERMAL_VALUES_HISTORY; i++)
            temperature_history[i] = temp;
    } else {  // update our current temperature estimate
        // crude lowpass filter
        // (limit rate of change to 1 degree per measurement)
        if (temp > temperature) {
            temperature ++;
        } else if (temp < temperature) {
            temperature --;
        }
    }

    // guess what the temperature will be in a few seconds
    int16_t pt;
    {
        int16_t diff;
        int16_t t = temperature;

        // algorithm tweaking; not really intended to be modified
        // how far ahead should we predict?
        #ifndef THERM_PREDICTION_STRENGTH
        #define THERM_PREDICTION_STRENGTH 4
        #endif
        // how proportional should the adjustments be?  (not used yet)
        #ifndef THERM_RESPONSE_MAGNITUDE
        #define THERM_RESPONSE_MAGNITUDE 128
        #endif
        // acceptable temperature window size in C
        #define THERM_WINDOW_SIZE 5
        // highest temperature allowed
        #define THERM_CEIL ((int16_t)therm_ceil)
        // bottom of target temperature window
        #define THERM_FLOOR (THERM_CEIL - THERM_WINDOW_SIZE)

        // if it's time to rotate the thermal history, do it
        history_step ++;
        #if (THERMAL_UPDATE_SPEED == 4)  // new value every 4s
        #define THERM_HISTORY_STEP_MAX 15
        #elif (THERMAL_UPDATE_SPEED == 2)  // new value every 2s
        #define THERM_HISTORY_STEP_MAX 7
        #elif (THERMAL_UPDATE_SPEED == 1)  // new value every 1s
        #define THERM_HISTORY_STEP_MAX 3
        #elif (THERMAL_UPDATE_SPEED == 0)  // new value every 0.5s
        #define THERM_HISTORY_STEP_MAX 1
        #endif
        if (0 == (history_step & THERM_HISTORY_STEP_MAX)) {
            // rotate measurements and add a new one
            for (uint8_t i=0; i<NUM_THERMAL_VALUES_HISTORY-1; i++) {
                temperature_history[i] = temperature_history[i+1];
            }
            temperature_history[NUM_THERMAL_VALUES_HISTORY-1] = t;
        }

        // guess what the temp will be several seconds in the future
        // diff = rate of temperature change
        //diff = temperature_history[NUM_THERMAL_VALUES_HISTORY-1] - temperature_history[0];
        diff = t - temperature_history[0];
        // slight bias toward zero; ignore very small changes (noise)
        for (uint8_t z=0; z<3; z++) {
            if (diff < 0) diff ++;
            if (diff > 0) diff --;
        }
        // projected_temperature = current temp extended forward by amplified rate of change
        //projected_temperature = temperature_history[NUM_THERMAL_VALUES_HISTORY-1] + (diff<<THERM_PREDICTION_STRENGTH);
        pt = projected_temperature = t + (diff<<THERM_PREDICTION_STRENGTH);
    }

    // cancel counters if appropriate
    if (pt > THERM_FLOOR) {
        underheat_lowpass = 0;  // we're probably not too cold
    }
    if (pt < THERM_CEIL) {
        overheat_lowpass = 0;  // we're probably not too hot
    }

    if (temperature_timer) {
        temperature_timer --;
    } else {  // it has been long enough since the last warning

        // Too hot?
        if (pt > THERM_CEIL) {
            if (overheat_lowpass < OVERHEAT_LOWPASS_STRENGTH) {
                overheat_lowpass ++;
            } else {
                // reset counters
                overheat_lowpass = 0;
                temperature_timer = TEMPERATURE_TIMER_START;
                // how far above the ceiling?
                //int16_t howmuch = (pt - THERM_CEIL) * THERM_RESPONSE_MAGNITUDE / 128;
                int16_t howmuch = pt - THERM_CEIL;
                // try to send out a warning
                emit(EV_temperature_high, howmuch);
            }
        }

        // Too cold?
        else if (pt < THERM_FLOOR) {
            if (underheat_lowpass < UNDERHEAT_LOWPASS_STRENGTH) {
                underheat_lowpass ++;
            } else {
                // reset counters
                underheat_lowpass = 0;
                temperature_timer = TEMPERATURE_TIMER_START;
                // how far below the floor?
                //int16_t howmuch = (THERM_FLOOR - pt) * THERM_RESPONSE_MAGNITUDE / 128;
                int16_t howmuch = THERM_FLOOR - pt;
                // try to send out a warning (unless voltage is low)
                // (LVP and underheat warnings fight each other)
                if (voltage > VOLTAGE_LOW)
                    emit(EV_temperature_low, howmuch);
            }
        }
    }
}
#endif


#ifdef USE_BATTCHECK
#ifdef BATTCHECK_4bars
PROGMEM const uint8_t voltage_blinks[] = {
    30, 35, 38, 40, 42, 99,
};
#endif
#ifdef BATTCHECK_6bars
PROGMEM const uint8_t voltage_blinks[] = {
    30, 34, 36, 38, 40, 41, 43, 99,
};
#endif
#ifdef BATTCHECK_8bars
PROGMEM const uint8_t voltage_blinks[] = {
    30, 33, 35, 37, 38, 39, 40, 41, 42, 99,
};
#endif
void battcheck() {
    #ifdef BATTCHECK_VpT
    blink_num(voltage);
    #else
    uint8_t i;
    for(i=0;
        voltage >= pgm_read_byte(voltage_blinks + i);
        i++) {}
    #ifdef DONT_DELAY_AFTER_BATTCHECK
    blink_digit(i);
    #else
    if (blink_digit(i))
        nice_delay_ms(1000);
    #endif
    #endif
}
#endif

#endif
