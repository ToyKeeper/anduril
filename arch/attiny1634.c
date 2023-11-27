// arch/attiny1634.c: attiny85 support functions
// Copyright (C) 2014-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "arch/attiny1634.h"

////////// clock speed / delay stuff //////////

inline void mcu_clock_speed() {
    // TODO?
    // (or not; clock speed is set by the fuses)
    // attiny1634 datasheet 6.5
    // CLKSR = [OSCRDY, CSTR, CKOUT_IO, SUT, CKSEL3/2/1/0]
    // default 8MHz calibrated internal clock: SUT=0, CKSEL=0b0010
    #if 0
    cli();
    CCP = 0xD8;
    CLKSR = 0b01000010;
    CCP = 0xD8;
    CLKPR = 0;  // CLK / 1 = full speed, 8 MHz
    sei();
    #endif
}

///// clock dividers
inline void clock_prescale_set(uint8_t n) {
    cli();
    // _PROTECTED_WRITE(CLKPR, n);
    CCP = 0xD8;
    CLKPR = n;
    sei();
}

////////// default hw_setup() //////////


////////// ADC voltage / temperature //////////

inline void mcu_set_admux_therm() {
    // put the ADC in temperature mode
    // DS table 19-3, 19-4, internal sensor / 1.1V ref
    // [refs1, refs0, refen, adc0en, mux3, mux2, mux1, mux0]
    // refs=0b10 : internal 1.1V ref
    // mux=0b1110 : internal temperature sensor
    //#define ADMUX_THERM 0b10001110
    ADMUX = ADMUX_THERM;
    // other stuff is already set, so no need to re-set it
}

inline void mcu_set_admux_voltage() {
    // put the ADC in battery voltage measurement mode
    // TODO: avr datasheet references
    #ifdef USE_VOLTAGE_DIVIDER  // 1.1V / pin7
        ADMUX = ADMUX_VOLTAGE_DIVIDER;
        // disable digital input on divider pin to reduce power consumption
        // TODO: this should be in hwdef init, not here
        VOLTAGE_ADC_DIDR |= (1 << VOLTAGE_ADC);
    #else  // VCC / 1.1V reference
        ADMUX = ADMUX_VCC;
        // disable digital input on VCC pin to reduce power consumption
        //VOLTAGE_ADC_DIDR |= (1 << VOLTAGE_ADC);  // FIXME: unsure how to handle for VCC pin
    #endif
    //ACSRA |= (1 << ACD);  // turn off analog comparator to save power
    // ADCSRB: [VDEN, VDPD, -, -, ADLAR, ADTS2, ADTS1, ADTS0]
    ADCSRB = (1 << ADLAR);  // left-adjust, free-running
    //ADCSRB = 0;  // right-adjust, free-running
}

inline void mcu_adc_sleep_mode() {
    set_sleep_mode(SLEEP_MODE_ADC);
}

inline void mcu_adc_start_measurement() {
    // [ADEN, ADSC, ADATE, adif, ADIE, ADPS2, ADPS1, ADPS0]
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
//inline uint16_t mcu_adc_result() { return (uint16_t)(ADCL | (ADCH << 8)); }
// right-adjusted mode:
/*
inline uint16_t mcu_adc_result() {
    uint16_t result = (ADCL | (ADCH << 8)) << 6;
    return result;
}
*/

inline uint8_t mcu_vdd_raw2cooked(uint16_t measurement) {
    // In : 65535 * 1.1 / Vbat
    // Out: uint8_t: Vbat * 40
    // 1.1 = ADC Vref
    #if 0
    // 1024 = how much ADC resolution we're using (10 bits)
    // (12 bits available, but it costs an extra 84 bytes of ROM to calculate)
    uint8_t vbat40 = (uint16_t)(40 * 1.1 * 1024) / (measurement >> 6);
    #else
    // ... spend the extra 84 bytes of ROM for better precision
    // 4096 = how much ADC resolution we're using (12 bits)
    uint8_t vbat40 = (uint32_t)(40 * 1.1 * 4096) / (measurement >> 4);
    #endif
    return vbat40;
}


#ifdef USE_VOLTAGE_DIVIDER
inline uint8_t mcu_vdivider_raw2cooked(uint16_t measurement) {
    // In : 4095 * Vdiv / 1.1V
    // Out: uint8_t: Vbat * 40
    // Vdiv = Vbat / 4.3  (typically)
    // 1.1 = ADC Vref
    const uint16_t adc_per_volt =
            (((uint16_t)ADC_44 << 4) - ((uint16_t)ADC_22 << 4))
            / (4 * (44-22));
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
    // right-adjusted mode:
    // datasheet 19.13.3.2:
    // "When ADCL is read, the ADC Data Register is not updated
    //  until ADCH is read. ... ADCL must be read first, then ADCH."
    // So... gotta read it even if not needed?
    // (the value doesn't matter here, the lower bits are only used
    //  for generating some random seed data)
    //return ADCL + ADCH;
}


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
    // _PROTECTED_WRITE(WDTCSR, 0);
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
    // _PROTECTED_WRITE(WDTCSR, 0b10001000);
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
    wdt_disable();  // from avr/wdt.h
}

