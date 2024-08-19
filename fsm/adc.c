// fsm-adc.c: ADC (voltage, temperature) functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// override onboard temperature sensor definition, if relevant
#ifdef USE_EXTERNAL_TEMP_SENSOR
#ifdef ADMUX_THERM
#undef ADMUX_THERM
#endif
#define ADMUX_THERM ADMUX_THERM_EXTERNAL_SENSOR
#endif

#include <avr/sleep.h>


static inline void adc_therm_mode() {
    hwdef_set_admux_therm();
    adc_channel = 1;
    adc_sample_count = 0;  // first result is unstable
    ADC_start_measurement();
}

void adc_voltage_mode() {
    hwdef_set_admux_voltage();
    adc_channel = 0;
    adc_sample_count = 0;  // first result is unstable
    ADC_start_measurement();
}


#if 0
#ifdef USE_VOLTAGE_DIVIDER
static inline uint8_t calc_voltage_divider(uint16_t value) {
    // use 9.7 fixed-point to get sufficient precision
    uint16_t adc_per_volt = ((ADC_44<<5) - (ADC_22<<5)) / (44-22);
    // shift incoming value into a matching position
    uint8_t result = ((value / adc_per_volt)
                     + VOLTAGE_FUDGE_FACTOR
                     #ifdef USE_VOLTAGE_CORRECTION
                        + VOLT_CORR - 7
                     #endif
                     ) >> 1;
    return result;
}
#endif
#endif

// Each full cycle runs ~2X per second with just voltage enabled,
// or ~1X per second with voltage and temperature.
#if defined(USE_LVP) && defined(USE_THERMAL_REGULATION)
#define ADC_CYCLES_PER_SECOND 1
#else
#define ADC_CYCLES_PER_SECOND 2
#endif

// happens every time the ADC sampler finishes a measurement
ISR(ADC_vect) {

    // clear the interrupt flag
    mcu_adc_vect_clear();

    if (adc_sample_count) {

        uint16_t m;  // latest measurement
        uint16_t s;  // smoothed measurement
        uint8_t channel = adc_channel;

        // update the latest value
        #ifdef MCU_ADC_RESULT_PER_TYPE
            if (channel) m = mcu_adc_result_temp();
            else m = mcu_adc_result_volts();
        #else
            m = mcu_adc_result();
        #endif
        adc_raw[channel] = m;

        // lowpass the value
        //s = adc_smooth[channel];  // easier to read
        uint16_t *v = adc_smooth + channel;  // compiles smaller
        s = *v;
        if (m > s) { s++; }
        if (m < s) { s--; }
        //adc_smooth[channel] = s;
        *v = s;

        // track what woke us up, and enable deferred logic
        irq_adc = 1;

    }

    // the next measurement isn't the first
    adc_sample_count = 1;
    // rollover doesn't really matter
    //adc_sample_count ++;

}

void adc_deferred() {
    irq_adc = 0;  // event handled

    #ifdef USE_PSEUDO_RAND
    // real-world entropy makes this a true random, not pseudo
    // Why here instead of the ISR?  Because it makes the time-critical ISR
    // code a few cycles faster and we don't need crypto-grade randomness.
    pseudo_rand_seed += mcu_adc_lsb();
    #endif

    // the ADC triggers repeatedly when it's on, but we only need to run the
    // voltage and temperature regulation stuff once in a while...so disable
    // this after each activation, until it's manually enabled again
    if (! adc_deferred_enable) return;

    // disable after one iteration
    adc_deferred_enable = 0;

    // what is being measured? 0 = battery voltage, 1 = temperature
    uint8_t adc_step;

    #if defined(USE_LVP) && defined(USE_THERMAL_REGULATION)
    // do whichever one is currently active
    adc_step = adc_channel;
    #else
    // unless there's no temperature sensor...  then just do voltage
    adc_step = 0;
    #endif

    #if defined(TICK_DURING_STANDBY) && defined(USE_SLEEP_LVP)
        // in sleep mode, turn off after just one measurement
        // (having the ADC on raises standby power by about 250 uA)
        // (and the usual standby level is only ~20 uA)
        if (go_to_standby) {
            ADC_off();
            // if any measurements were in progress, they're done now
            adc_active_now = 0;
            // also, only check the battery while asleep, not the temperature
            adc_channel = 0;
        }
    #endif

    if (0) {} // placeholder for easier syntax

    #ifdef USE_LVP
    else if (0 == adc_step) {  // voltage
        ADC_voltage_handler();
        #ifdef USE_THERMAL_REGULATION
        // set the correct type of measurement for next time
        if (! go_to_standby) adc_therm_mode();
        #endif
    }
    #endif

    #ifdef USE_THERMAL_REGULATION
    else if (1 == adc_step) {  // temperature
        ADC_temperature_handler();
        #ifdef USE_LVP
        // set the correct type of measurement for next time
        adc_voltage_mode();
        #endif
    }
    #endif

    if (adc_reset) adc_reset --;
}


#ifdef USE_LVP
static void ADC_voltage_handler() {
    // rate-limit low-voltage warnings to a max of 1 per N seconds
    static uint8_t lvp_timer = 0;
    #define LVP_TIMER_START (VOLTAGE_WARNING_SECONDS*ADC_CYCLES_PER_SECOND)  // N seconds between LVP warnings

    #ifdef NO_LVP_WHILE_BUTTON_PRESSED
    // don't run if button is currently being held
    // (because the button causes a reading of zero volts)
    if (button_last_state) return;
    #endif

    uint16_t measurement;

    // latest ADC value
    if (adc_reset) {  // just after waking, don't lowpass
        measurement = adc_raw[0];
        adc_smooth[0] = measurement;  // no lowpass, just use the latest value
    }
    #ifdef USE_LOWPASS_WHILE_ASLEEP
    else if (go_to_standby) {  // weaker lowpass while asleep
        // occasionally the aux LED color can oscillate during standby,
        // while using "voltage" mode ... so try to reduce the oscillation
        uint16_t r = adc_raw[0];
        uint16_t s = adc_smooth[0];
        #if 0
        // fixed-rate lowpass, stable but very slow
        // (move by only 0.5 ADC units per measurement, 1 ADC unit = 64)
        if (r < s) { s -= 32; }
        if (r > s) { s += 32; }
        #elif 1
        // 1/8th proportional lowpass, faster but less stable
        int16_t diff = (r/8) - (s/8);
        s += diff;
        #else
        // 50% proportional lowpass, fastest but least stable
        s = (r>>1) + (s>>1);
        #endif
        adc_smooth[0] = s;
        measurement = s;
    }
    #endif
    else measurement = adc_smooth[0];

    // convert raw ADC value to FSM voltage units: Volts * 50
    // 0 .. 250 = 0.0V .. 5.0V
    voltage = voltage_raw2cooked(measurement)
              + (VOLTAGE_FUDGE_FACTOR << 1)
              #ifdef USE_VOLTAGE_CORRECTION
                 + ((VOLT_CORR - 7) << 1)
              #endif
              ;

    // if low, callback EV_voltage_low / EV_voltage_critical
    //         (but only if it has been more than N seconds since last call)
    if (lvp_timer) {
        lvp_timer --;
    } else {  // it has been long enough since the last warning
        #ifdef DUAL_VOLTAGE_FLOOR
        if (((voltage < VOLTAGE_LOW) && (voltage > DUAL_VOLTAGE_FLOOR)) || (voltage < DUAL_VOLTAGE_LOW_LOW)) {
        #else
        if (voltage < VOLTAGE_LOW) {
        #endif
            // send out a warning
            emit(EV_voltage_low, 0);
            // reset rate-limit counter
            lvp_timer = LVP_TIMER_START;
        }
    }
}
#endif


#ifdef USE_THERMAL_REGULATION
// generally happens once per second while awake
static void ADC_temperature_handler() {
    // coarse adjustment
    #ifndef THERM_LOOKAHEAD
    #define THERM_LOOKAHEAD 4
    #endif
    // reduce frequency of minor warnings
    #ifndef THERM_NEXT_WARNING_THRESHOLD
    #define THERM_NEXT_WARNING_THRESHOLD 24
    #endif
    // fine-grained adjustment
    // how proportional should the adjustments be?
    #ifndef THERM_RESPONSE_MAGNITUDE
    #define THERM_RESPONSE_MAGNITUDE 64
    #endif
    // acceptable temperature window size in C
    #define THERM_WINDOW_SIZE 2

    // TODO? make this configurable per build target?
    //       (shorter time for hosts with a lower power-to-mass ratio)
    //       (because then it'll have smaller responses)
    #define NUM_TEMP_HISTORY_STEPS 8  // don't change; it'll break stuff
    static uint8_t history_step = 0;
    static uint16_t temperature_history[NUM_TEMP_HISTORY_STEPS];
    static int8_t warning_threshold = 0;

    if (adc_reset) adc_smooth[1] = adc_raw[1];

    // latest 16-bit ADC reading
    // convert raw ADC value to Kelvin << 6
    // 0 .. 65535 = 0 K .. 1024 K
    uint16_t measurement = temp_raw2cooked(adc_smooth[1]);

    // let the UI see the current temperature in C
    // (Kelvin << 6) to Celsius
    // Why 275?  Because Atmel's docs use 275 instead of 273.
    temperature = (measurement>>6) + THERM_CAL_OFFSET + (int16_t)TH_CAL - 275;

    // instead of (K << 6), use (K << 1) now
    // TODO: use more precision, if it can be done without overflow in 16 bits
    //       (and still work on attiny85 without increasing ROM size)
    #if 1
    measurement = measurement >> 5;
    #else  // TODO: is this still needed?
    // values stair-step between intervals of 64, with random variations
    // of 1 or 2 in either direction, so if we chop off the last 6 bits
    // it'll flap between N and N-1...  but if we add half an interval,
    // the values should be really stable after right-alignment
    // (instead of 99.98, 100.00, and 100.02, it'll hit values like
    //  100.48, 100.50, and 100.52...  which are stable when truncated)
    //measurement += 32;
    measurement = (measurement + 16) >> 5;
    //measurement = (measurement + 16) & 0xffe0;  // 1111 1111 1110 0000
    #endif

    if (adc_reset) {  // wipe out old data after waking up
        // forget any past measurements
        for(uint8_t i=0; i<NUM_TEMP_HISTORY_STEPS; i++)
            temperature_history[i] = measurement;
    }

    // how much has the temperature changed between now and a few seconds ago?
    int16_t diff;
    diff = measurement - temperature_history[history_step];

    // update / rotate the temperature history
    temperature_history[history_step] = measurement;
    history_step = (history_step + 1) & (NUM_TEMP_HISTORY_STEPS-1);

    // PI[D]: guess what the temperature will be in a few seconds
    uint16_t pt;  // predicted temperature
    pt = measurement + (diff * THERM_LOOKAHEAD);

    // convert temperature limit from C to raw 16-bit ADC units
    // C = (ADC>>6) - 275 + THERM_CAL_OFFSET + TH_CAL;
    // ... so ...
    // (C + 275 - THERM_CAL_OFFSET - TH_CAL) << 6 = ADC;
    uint16_t ceil = (TH_CEIL + 275 - TH_CAL - THERM_CAL_OFFSET) << 1;
    int16_t offset = pt - ceil;

    // bias small errors toward zero, while leaving large errors mostly unaffected
    // (a diff of 1 C is 2 ADC units, * 4 for therm lookahead, so it becomes 8)
    // (but a diff of 1 C should only send a warning of magnitude 1)
    // (this also makes it only respond to small errors at the time the error
    // happened, not after the temperature has stabilized)
    for(uint8_t foo=0; foo<3; foo++) {
        if (offset > 0) {
            offset --;
        } else if (offset < 0) {
            offset ++;
        }
    }

    // Too hot?
    // (if it's too hot and not getting cooler...)
    if ((offset > 0) && (diff > -1)) {
        // accumulated error isn't big enough yet to send a warning
        if (warning_threshold > 0) {
            warning_threshold -= offset;
        } else {  // error is big enough; send a warning
            // how far above the ceiling?
            // original method works, but is too slow on some small hosts:
            // (and typically has a minimum response magnitude of 2 instead of 1)
            //   int16_t howmuch = offset;
            // ... so increase the amount, except for small values
            // (for example, 1:1, 2:1, 3:3, 4:5, 6:9, 8:13, 10:17, 40:77)
            // ... and let us tune the response per build target if desired
            int16_t howmuch = (offset + offset - 3) * THERM_RESPONSE_MAGNITUDE / 128;
            if (howmuch < 1) howmuch = 1;
            warning_threshold = THERM_NEXT_WARNING_THRESHOLD - (uint8_t)howmuch;

            // send a warning
            emit(EV_temperature_high, howmuch);
        }
    }

    // Too cold?
    // (if it's too cold and still getting colder...)
    // the temperature is this far below the floor:
    #define BELOW (offset + (THERM_WINDOW_SIZE<<1))
    else if ((BELOW < 0) && (diff < 0)) {
        // accumulated error isn't big enough yet to send a warning
        if (warning_threshold < 0) {
            warning_threshold -= BELOW;
        } else {  // error is big enough; send a warning
            warning_threshold = (-THERM_NEXT_WARNING_THRESHOLD) - BELOW;

            // how far below the floor?
            // int16_t howmuch = ((-BELOW) >> 1) * THERM_RESPONSE_MAGNITUDE / 128;
            int16_t howmuch = (-BELOW) >> 1;
            // send a notification (unless voltage is low)
            // (LVP and underheat warnings fight each other)
            if (voltage > (VOLTAGE_LOW + 1))
                emit(EV_temperature_low, howmuch);
        }
    }
    #undef BELOW

    // Goldilocks?
    // (temperature is within target window, or at least heading toward it)
    else {
        // send a notification (unless voltage is low)
        // (LVP and temp-okay events fight each other)
        if (voltage > VOLTAGE_LOW)
            emit(EV_temperature_okay, 0);
    }
}
#endif


#ifdef USE_BATTCHECK
#ifdef BATTCHECK_4bars
PROGMEM const uint8_t voltage_blinks[] = {
    30*dV,
    35*dV,
    38*dV,
    40*dV,
    42*dV,
    255,
};
#endif
#ifdef BATTCHECK_6bars
PROGMEM const uint8_t voltage_blinks[] = {
    30*dV,
    34*dV,
    36*dV,
    38*dV,
    40*dV,
    41*dV,
    43*dV,
    255,
};
#endif
#ifdef BATTCHECK_8bars
PROGMEM const uint8_t voltage_blinks[] = {
    30*dV,
    33*dV,
    35*dV,
    37*dV,
    38*dV,
    39*dV,
    40*dV,
    41*dV,
    42*dV,
    255,
};
#endif
void battcheck() {
    #ifdef BATTCHECK_VpT
        blink_num(voltage / dV);
        #ifdef USE_EXTRA_BATTCHECK_DIGIT
            // 0.02V precision, 0 1 2 3 4 remainder -> .00 .02 .04 .06 .08V
            blink_num((voltage % dV) * (10/dV));
        #endif
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

