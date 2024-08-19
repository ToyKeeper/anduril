// arch/attiny85.c: attiny85 support functions
// Copyright (C) 2014-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "arch/attiny85.h"

////////// clock speed / delay stuff //////////

///// clock dividers

////////// default hw_setup() //////////

// FIXME: fsm/main should call hwdef_setup(), not hw_setup,
//        and this function should be hwdef_setup
#ifdef USE_GENERIC_HWDEF_SETUP
static inline void hwdef_setup() {
    // configure PWM channels
    #if PWM_CHANNELS >= 1
        DDRB |= (1 << PWM1_PIN);
        TCCR0B = 0x01; // pre-scaler for timer (1 => 1, 2 => 8, 3 => 64...)
        TCCR0A = PHASE;
        #if (PWM1_PIN == PB4) // Second PWM counter is ... weird
            TCCR1 = _BV (CS10);
            GTCCR = _BV (COM1B1) | _BV (PWM1B);
            OCR1C = 255;  // Set ceiling value to maximum
        #endif
    #endif
    // tint ramping needs second channel enabled,
    // despite PWM_CHANNELS being only 1
    #if (PWM_CHANNELS >= 2) || defined(USE_TINT_RAMPING)
        DDRB |= (1 << PWM2_PIN);
        #if (PWM2_PIN == PB4) // Second PWM counter is ... weird
            TCCR1 = _BV (CS10);
            GTCCR = _BV (COM1B1) | _BV (PWM1B);
            OCR1C = 255;  // Set ceiling value to maximum
        #endif
    #endif
    #if PWM_CHANNELS >= 3
        DDRB |= (1 << PWM3_PIN);
        #if (PWM3_PIN == PB4) // Second PWM counter is ... weird
            TCCR1 = _BV (CS10);
            GTCCR = _BV (COM1B1) | _BV (PWM1B);
            OCR1C = 255;  // Set ceiling value to maximum
        #endif
    #endif
    #if PWM_CHANNELS >= 4
        // 4th PWM channel is ... not actually supported in hardware  :(
        DDRB |= (1 << PWM4_PIN);
        //OCR1C = 255;  // Set ceiling value to maximum
        TCCR1 = 1<<CTC1 | 1<<PWM1A | 3<<COM1A0 | 2<<CS10;
        GTCCR = (2<<COM1B0) | (1<<PWM1B);
        // set up an interrupt to control PWM4 pin
        TIMSK |= (1<<OCIE1A) | (1<<TOIE1);
    #endif

    // configure e-switch
    PORTB = (1 << SWITCH_PIN);  // e-switch is the only input
    PCMSK = (1 << SWITCH_PIN);  // pin change interrupt uses this pin
}
#endif  // #ifdef USE_GENERIC_HWDEF_SETUP


////////// ADC voltage / temperature //////////

inline void mcu_set_admux_therm() {
    // put the ADC in temperature mode
    ADMUX = ADMUX_THERM | (1 << ADLAR);
}

inline void mcu_set_admux_voltage() {
    // put the ADC in battery voltage measurement mode
    // TODO: avr datasheet references
    #ifdef USE_VOLTAGE_DIVIDER  // 1.1V / pin7
        ADMUX = ADMUX_VOLTAGE_DIVIDER | (1 << ADLAR);
        // disable digital input on divider pin to reduce power consumption
        VOLTAGE_ADC_DIDR |= (1 << VOLTAGE_ADC);
    #else  // VCC / 1.1V reference
        ADMUX = ADMUX_VCC | (1 << ADLAR);
        // disable digital input on VCC pin to reduce power consumption
        //VOLTAGE_ADC_DIDR |= (1 << VOLTAGE_ADC);  // FIXME: unsure how to handle for VCC pin
    #endif
}

inline void mcu_adc_sleep_mode() {
    set_sleep_mode(SLEEP_MODE_ADC);
}

inline void mcu_adc_start_measurement() {
    ADCSRA = (1 << ADEN)   // enable
           | (1 << ADSC)   // start
           | (1 << ADATE)  // auto-retrigger
           | (1 << ADIE)   // interrupt enable
           | ADC_PRSCL;    // prescale
}

inline void mcu_adc_off() {
    ADCSRA &= ~(1<<ADEN); //ADC off
}

// left-adjusted mode:
inline uint16_t mcu_adc_result() { return ADC; }

inline uint8_t mcu_vdd_raw2cooked(uint16_t measurement) {
    // In : 65535 * 1.1 / Vbat
    // Out: uint8_t: Vbat * 50
    // 1.1 = ADC Vref
    // 1024 = how much ADC resolution we're using (10 bits)
    // (12 bits available, but it costs an extra 84 bytes of ROM to calculate)
    uint8_t vbat = (uint16_t)(10 * dV * 1.1 * 1024) / (measurement >> 6);
    return vbat;
}


#ifdef USE_VOLTAGE_DIVIDER
inline uint8_t mcu_vdivider_raw2cooked(uint16_t measurement) {
    // In : 4095 * Vdiv / 1.1V
    // Out: uint8_t: Vbat * 50
    // Vdiv = Vbat / 4.3  (typically)
    // 1.1 = ADC Vref
    const uint16_t adc_per_volt =
            (((uint16_t)ADC_44 << 4) - ((uint16_t)ADC_22 << 4))
            / (dV * (44-22));
    uint8_t result = measurement / adc_per_volt;
    return result;
}
#endif

inline uint16_t mcu_temp_raw2cooked(uint16_t measurement) {
    // convert raw ADC values to calibrated temperature
    // In: ADC raw temperature (16-bit, or left-aligned)
    // Out: Kelvin << 6
    // Precision: 1/64th Kelvin (but noisy)
    // attiny1634 datasheet section 19.12
    // nothing to do; input value is already "cooked"
    return measurement;
}

inline uint8_t mcu_adc_lsb() {
    // left-adjusted mode:
    return (ADCL >> 6) + (ADCH << 2);
}


////////// WDT //////////

inline void mcu_wdt_active() {
    // interrupt every 16ms
    //cli();                          // Disable interrupts
    wdt_reset();                    // Reset the WDT
    WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
    WDTCR = (1<<WDIE);              // Enable interrupt every 16ms
    //sei();                          // Enable interrupts
}

inline void mcu_wdt_standby() {
    // interrupt slower
    //cli();                          // Disable interrupts
    wdt_reset();                    // Reset the WDT
    WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
    WDTCR = (1<<WDIE) | STANDBY_TICK_SPEED; // Enable interrupt every so often
    //sei();                          // Enable interrupts
}

inline void mcu_wdt_stop() {
    //cli();                          // Disable interrupts
    wdt_reset();                    // Reset the WDT
    MCUSR &= ~(1<<WDRF);            // Clear Watchdog reset flag
    WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
    WDTCR = 0x00;                   // Disable WDT
    //sei();                          // Enable interrupts
}


////////// PCINT - pin change interrupt (e-switch) //////////

inline void mcu_pcint_on() {
    // enable pin change interrupt
    GIMSK |= (1 << PCIE);
    // only pay attention to the e-switch pin
    #if 0  // this is redundant; was already done in main()
    PCMSK = (1 << SWITCH_PCINT);
    #endif
    // set bits 1:0 to 0b01 (interrupt on rising *and* falling edge) (default)
    // MCUCR &= 0b11111101;  MCUCR |= 0b00000001;
}

inline void mcu_pcint_off() {
    // disable all pin-change interrupts
    GIMSK &= ~(1 << PCIE);
}


////////// misc //////////

void reboot() {
    // put the WDT in hard reset mode, then trigger it
    cli();
    WDTCR = 0xD8 | WDTO_15MS;
    sei();
    wdt_reset();
    while (1) {}
}

inline void prevent_reboot_loop() {
    // prevent WDT from rebooting MCU again
    MCUSR &= ~(1<<WDRF);  // reset status flag
    wdt_disable();
}


#if 0  // example for one way of creating a 4th PWM channel
// 4th PWM channel requires manually turning the pin on/off via interrupt :(
ISR(TIMER1_OVF_vect) {
    //bitClear(PORTB, 3);
    PORTB &= 0b11110111;
    //PORTB |= 0b00001000;
}
ISR(TIMER1_COMPA_vect) {
    //if (!bitRead(TIFR,TOV1)) bitSet(PORTB, 3);
    if (! (TIFR & (1<<TOV1))) PORTB |= 0b00001000;
    //if (! (TIFR & (1<<TOV1))) PORTB &= 0b11110111;
}
#endif

