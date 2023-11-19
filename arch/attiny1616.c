// arch/attiny1616.c: attiny1616 support functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "arch/attiny1616.h"

////////// clock speed / delay stuff //////////

///// clock dividers
// this should work, but needs further validation
inline void clock_prescale_set(uint8_t n) {
    cli();
    CCP = CCP_IOREG_gc; // temporarily disable clock change protection
    CLKCTRL.MCLKCTRLB = n; // Set the prescaler
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {} // wait for clock change to finish
    sei();
}

////////// ADC voltage / temperature //////////

inline void mcu_set_admux_therm() {
    ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;  // read temperature
    ADC0.CTRLC = ADC_SAMPCAP_bm
               | ADC_PRESC_DIV64_gc
               | ADC_REFSEL_INTREF_gc; // Internal ADC reference
}

inline void mcu_set_admux_voltage() {
    #ifdef USE_VOLTAGE_DIVIDER  // 1.1V / ADC input pin
        // verify that this is correct!!!  untested
        ADC0.MUXPOS = ADMUX_VOLTAGE_DIVIDER;  // read the requested ADC pin
        ADC0.CTRLC = ADC_SAMPCAP_bm
                   | ADC_PRESC_DIV64_gc
                   | ADC_REFSEL_INTREF_gc; // Use internal ADC reference
    #else  // VCC / 1.1V reference
        ADC0.MUXPOS = ADC_MUXPOS_INTREF_gc;  // read internal reference
        ADC0.CTRLC = ADC_SAMPCAP_bm
                   | ADC_PRESC_DIV64_gc
                   | ADC_REFSEL_VDDREF_gc; // Vdd (Vcc) be ADC reference
    #endif
}

inline void mcu_adc_sleep_mode() {
    set_sleep_mode(SLEEP_MODE_STANDBY);
}

inline void mcu_adc_start_measurement() {
    ADC0.INTCTRL |= ADC_RESRDY_bm; // enable interrupt
    ADC0.COMMAND |= ADC_STCONV_bm; // Start the ADC conversions
}

inline void mcu_adc_on() {
    VREF.CTRLA |= VREF_ADC0REFSEL_1V1_gc; // Set Vbg ref to 1.1V
    // Enabled, free-running (aka, auto-retrigger), run in standby
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_FREERUN_bm | ADC_RUNSTBY_bm;
    // set a INITDLY value because the AVR manual says so (section 30.3.5)
    // (delay 1st reading until Vref is stable)
    ADC0.CTRLD |= ADC_INITDLY_DLY16_gc;
    hwdef_set_admux_voltage();
}

inline void mcu_adc_off() {
    ADC0.CTRLA &= ~(ADC_ENABLE_bm);  // disable the ADC
}

inline void mcu_adc_vect_clear() {
    ADC0.INTFLAGS = ADC_RESRDY_bm; // clear the interrupt
}

inline uint16_t mcu_adc_result_temp() {
    // Use the factory calibrated values in SIGROW.TEMPSENSE0 and
    // SIGROW.TEMPSENSE1 to calculate a temperature reading in Kelvin, then
    // left-align it.
    int8_t sigrow_offset = SIGROW.TEMPSENSE1;  // Read signed value from signature row
    uint8_t sigrow_gain = SIGROW.TEMPSENSE0;  // Read unsigned value from signature row
    uint32_t temp = ADC0.RES - sigrow_offset;
    temp *= sigrow_gain;  // Result might overflow 16 bit variable (10bit+8bit)
    temp += 0x80;  // Add 1/2 to get correct rounding on division below
    //temp >>= 8;  // Divide result to get Kelvin
    //return temp << 6;  // left align it
    return temp >> 2;  // left-aligned uint16_t
}

inline uint16_t mcu_adc_result_volts() {
    // FIXME: set up ADC to use left-aligned values natively
    return ADC0.RES << 6;  // voltage, force left-alignment
}

inline uint8_t mcu_adc_lsb() {
    //return (ADCL >> 6) + (ADCH << 2);
    return ADC0.RESL; // right aligned, not left... so should be equivalent?
}


////////// WDT //////////

inline void mcu_wdt_active() {
    RTC.PITINTCTRL = RTC_PI_bm;   // enable the Periodic Interrupt
    while (RTC.PITSTATUS > 0) {}  // make sure the register is ready to be updated
    RTC.PITCTRLA = RTC_PERIOD_CYC512_gc | RTC_PITEN_bm; // Period = 16ms, enable the PI Timer
}

inline void mcu_wdt_standby() {
    RTC.PITINTCTRL = RTC_PI_bm;   // enable the Periodic Interrupt
    while (RTC.PITSTATUS > 0) {}  // make sure the register is ready to be updated
    RTC.PITCTRLA = (1<<6) | (STANDBY_TICK_SPEED<<3) | RTC_PITEN_bm; // Set period, enable the PI Timer
}

inline void mcu_wdt_stop() {
    while (RTC.PITSTATUS > 0) {}  // make sure the register is ready to be updated
    RTC.PITCTRLA = 0; // Disable the PI Timer
}

inline void mcu_wdt_vect_clear() {
    RTC.PITINTFLAGS = RTC_PI_bm; // clear the PIT interrupt flag
}


////////// PCINT - pin change interrupt (e-switch) //////////

inline void mcu_switch_vect_clear() {
    // Write a '1' to clear the interrupt flag
    SWITCH_INTFLG |= (1 << SWITCH_PIN);
}

inline void mcu_pcint_on() {
    SWITCH_ISC_REG |= PORT_ISC_BOTHEDGES_gc;
}

inline void mcu_pcint_off() {
    SWITCH_ISC_REG &= ~(PORT_ISC_gm);
}


////////// misc //////////

void reboot() {
    // put the WDT in hard reset mode, then trigger it
    cli();
    CCP = CCP_IOREG_gc;  // temporarily disable change protection
    WDT.CTRLA = WDT_PERIOD_8CLK_gc;  // Enable, timeout 8ms
    sei();
    wdt_reset();
    while (1) {}
}

inline void prevent_reboot_loop() {
    // prevent WDT from rebooting MCU again
    RSTCTRL.RSTFR &= ~(RSTCTRL_WDRF_bm);  // reset status flag
    wdt_disable();
}

