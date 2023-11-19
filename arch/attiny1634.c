// arch/attiny1634.c: attiny85 support functions
// Copyright (C) 2014-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "arch/attiny1634.h"

////////// clock speed / delay stuff //////////

///// clock dividers
// make it a NOP for now
// FIXME
//#define clock_prescale_set(x) ((void)0)
//#define clock_prescale_set(n) {cli(); CCP = 0xD8; CLKPR = n; sei();}
//#define clock_prescale_set(n) {cli(); CCP = 0xD8; CLKPR = n; sei();}
inline void clock_prescale_set(uint8_t n) {
    cli();
    CCP = 0xD8;
    CLKPR = n;
    sei();
}

////////// default hw_setup() //////////


////////// ADC voltage / temperature //////////

inline void mcu_set_admux_therm() {
    ADMUX = ADMUX_THERM;
}

inline void mcu_set_admux_voltage() {
    #ifdef USE_VOLTAGE_DIVIDER  // 1.1V / pin7
        ADMUX = ADMUX_VOLTAGE_DIVIDER;
    #else  // VCC / 1.1V reference
        ADMUX = ADMUX_VCC;
    #endif
}

inline void mcu_adc_sleep_mode() {
    set_sleep_mode(SLEEP_MODE_ADC);
}

inline void mcu_adc_start_measurement() {
    ADCSRA |= (1 << ADSC) | (1 << ADIE);
}

inline void mcu_adc_on() {
    hwdef_set_admux_voltage();
    #ifdef USE_VOLTAGE_DIVIDER
        // disable digital input on divider pin to reduce power consumption
        VOLTAGE_ADC_DIDR |= (1 << VOLTAGE_ADC);
    #else
        // disable digital input on VCC pin to reduce power consumption
        //VOLTAGE_ADC_DIDR |= (1 << VOLTAGE_ADC);  // FIXME: unsure how to handle for VCC pin
    #endif
    //ACSRA |= (1 << ACD);  // turn off analog comparator to save power
    ADCSRB |= (1 << ADLAR);  // left-adjust flag is here instead of ADMUX
    // enable, start, auto-retrigger, prescale
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | ADC_PRSCL;
}

inline void mcu_adc_off() {
    ADCSRA &= ~(1<<ADEN); //ADC off
}

inline uint16_t mcu_adc_result() { return ADC; }

inline uint8_t mcu_adc_lsb() { return (ADCL >> 6) + (ADCH << 2); }


////////// WDT //////////

inline void mcu_wdt_active() {
    wdt_reset();                    // Reset the WDT
    WDTCSR = (1<<WDIE);             // Enable interrupt every 16ms
}

inline void mcu_wdt_standby() {
    wdt_reset();                    // Reset the WDT
    WDTCSR = (1<<WDIE) | STANDBY_TICK_SPEED;
}

inline void mcu_wdt_stop() {
    cli();                // needed because CCP, below
    wdt_reset();          // Reset the WDT
    MCUSR &= ~(1<<WDRF);  // clear watchdog reset flag
    CCP = 0xD8;           // enable config changes
    WDTCSR = 0;           // disable and clear all WDT settings
    sei();
}


////////// PCINT - pin change interrupt (e-switch) //////////

inline void mcu_pcint_on() {
    // enable pin change interrupt
    #ifdef SWITCH2_PCIE
    GIMSK |= ((1 << SWITCH_PCIE) | (1 << SWITCH2_PCIE));
    #else
    GIMSK |= (1 << SWITCH_PCIE);
    #endif
}

inline void mcu_pcint_off() {
    // disable all pin-change interrupts
    GIMSK &= ~(1 << SWITCH_PCIE);
}


////////// misc //////////

void reboot() {
    // put the WDT in hard reset mode, then trigger it
    cli();
    // allow protected configuration changes for next 4 clock cycles
    CCP = 0xD8;  // magic number
    // reset (WDIF + WDE), no WDIE, fastest (16ms) timing (0000)
    // (DS section 8.5.2 and table 8-4)
    WDTCSR = 0b10001000;
    sei();
    wdt_reset();
    while (1) {}
}

inline void prevent_reboot_loop() {
    // prevent WDT from rebooting MCU again
    MCUSR &= ~(1<<WDRF);  // reset status flag
    wdt_disable();
}

