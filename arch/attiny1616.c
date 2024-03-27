// arch/attiny1616.c: attiny1616 support functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "arch/attiny1616.h"

////////// clock speed / delay stuff //////////

inline void mcu_clock_speed() {
    // TODO: allow hwdef to define a base clock speed
    // set up the system clock to run at 10 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB,
                      CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );
}

///// clock dividers
// this should work, but needs further validation
inline void clock_prescale_set(uint8_t n) {
    cli();
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, n); // Set the prescaler
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {} // wait for clock change to finish
    sei();
}


////////// ADC voltage / temperature //////////

inline void mcu_set_admux_therm() {
    // put the ADC in temperature mode
    // attiny1616 datasheet section 30.3.2.6
    mcu_set_adc0_vref(VREF_ADC0REFSEL_1V1_gc);  // Set Vbg ref to 1.1V
    ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;  // read temperature
    ADC0.CTRLB = ADC_SAMPNUM_ACC4_gc;  // 10-bit result + 4x oversampling
    ADC0.CTRLC = ADC_SAMPCAP_bm
               | ADC_PRESC_DIV16_gc
               | ADC_REFSEL_INTREF_gc; // Internal ADC reference
}

inline void mcu_set_admux_voltage() {
    // Enabled, free-running (aka, auto-retrigger), run in standby
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_FREERUN_bm | ADC_RUNSTBY_bm;
    // set a INITDLY value because the AVR manual says so (section 30.3.5)
    // (delay 1st reading until Vref is stable)
    ADC0.CTRLD |= ADC_INITDLY_DLY16_gc;
    #ifdef USE_VOLTAGE_DIVIDER  // measure an arbitrary pin
        // result = resolution * Vdiv / 1.1V
        mcu_set_adc0_vref(VREF_ADC0REFSEL_1V1_gc);  // Set Vbg ref to 1.1V
        ADC0.MUXPOS = ADMUX_VOLTAGE_DIVIDER;  // read the requested ADC pin
        ADC0.CTRLB = ADC_SAMPNUM_ACC4_gc;  // 12-bit result, 4x oversampling
        ADC0.CTRLC = ADC_SAMPCAP_bm
                   | ADC_PRESC_DIV16_gc
                   | ADC_REFSEL_INTREF_gc; // Use internal ADC reference
    #else  // measure VDD pin
        // result = resolution * 1.5V / Vbat
        mcu_set_adc0_vref(VREF_ADC0REFSEL_1V5_gc);  // Set Vbg ref to 1.5V
        ADC0.MUXPOS = ADC_MUXPOS_INTREF_gc;  // read internal reference
        ADC0.CTRLB = ADC_SAMPNUM_ACC4_gc;  // 12-bit result, 4x oversampling
        ADC0.CTRLC = ADC_SAMPCAP_bm
                   | ADC_PRESC_DIV16_gc
                   | ADC_REFSEL_VDDREF_gc; // Vdd (Vcc) be ADC reference
    #endif
}

inline void mcu_adc_sleep_mode() {
    set_sleep_mode(SLEEP_MODE_STANDBY);
}

inline void mcu_adc_start_measurement() {
    ADC0.INTCTRL |= ADC_RESRDY_bm; // enable interrupt
    ADC0.COMMAND |= ADC_STCONV_bm; // actually start measuring
}

/*
inline void mcu_adc_on() {
    VREF.CTRLA |= VREF_ADC0REFSEL_1V1_gc; // Set Vbg ref to 1.1V
    // Enabled, free-running (aka, auto-retrigger), run in standby
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_FREERUN_bm | ADC_RUNSTBY_bm;
    // set a INITDLY value because the AVR manual says so (section 30.3.5)
    // (delay 1st reading until Vref is stable)
    ADC0.CTRLD |= ADC_INITDLY_DLY16_gc;
    hwdef_set_admux_voltage();
}
*/

inline void mcu_adc_off() {
    ADC0.CTRLA &= ~(ADC_ENABLE_bm);  // disable the ADC
}

inline void mcu_adc_vect_clear() {
    ADC0.INTFLAGS = ADC_RESRDY_bm; // clear the interrupt
}

inline uint16_t mcu_adc_result_temp() {
    // just return left-aligned ADC result, don't convert to calibrated units
    //return ADC0.RES << 6;
    return ADC0.RES << 4;
}

inline uint16_t mcu_adc_result_volts() {
    // ADC has no left-aligned mode, so left-align it manually
    return ADC0.RES << 4;
}

inline uint8_t mcu_vdd_raw2cooked(uint16_t measurement) {
    // In : 65535 * 1.5 / Vbat
    // Out: uint8_t: Vbat * 50
    // 1.5 = ADC Vref
    #if 0
    // 1024 = how much ADC resolution we're using (10 bits)
    // (12 bits available, but it costs an extra 84 bytes of ROM to calculate)
    uint8_t vbat = (uint16_t)(10 * dV * 1.5 * 1024) / (measurement >> 6);
    #else
    // ... spend the extra 84 bytes of ROM for better precision
    // 4096 = how much ADC resolution we're using (12 bits)
    uint8_t vbat = (uint32_t)(10 * dV * 1.5 * 4096) / (measurement >> 4);
    #endif
    return vbat;
}

#if 0  // fine voltage, 0 to 10.24V in 1/6400th V steps
inline uint16_t mcu_vdd_raw2fine(uint16_t measurement) {
    // In : 65535 * 1.5 / Vbat
    // Out: 65535 * (Vbat / 10) / 1.024V
    uint16_t voltage = ((uint32_t)(1.5 * 4096 * 100 * 64 * 16) / measurement;
    return voltage;
}
#endif

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
    // In: ADC raw temperature (16-bit, or 12-bit left-aligned)
    // Out: Kelvin << 6
    // Precision: 1/64th Kelvin (but noisy)
    // attiny1616 datasheet section 30.3.2.6
    uint8_t sigrow_gain  = SIGROW.TEMPSENSE0;  // factory calibration data
    int8_t sigrow_offset = SIGROW.TEMPSENSE1;
    const uint32_t scaling_factor = 65536;  // use all 16 bits of ADC data
    uint32_t temp = measurement - (sigrow_offset << 6);
    temp *= sigrow_gain;  // 24-bit result
    temp += scaling_factor / 8;  // Add 1/8th K to get correct rounding on later divisions
    temp = temp >> 8;  // change (K << 14) to (K << 6)
    return temp;  // left-aligned uint16_t, 0 to 1023.98 Kelvin
}

inline uint8_t mcu_adc_lsb() {
    //return (ADCL >> 6) + (ADCH << 2);
    return ADC0.RESL; // right aligned, not left... so should be equivalent?
}


////////// WDT //////////

inline void mcu_wdt_active() {
    RTC.PITINTCTRL = RTC_PI_bm;   // enable the Periodic Interrupt
    while (RTC.PITSTATUS > 0) {}  // make sure the register is ready to be updated
    // Period = 16ms (64 Hz), enable the PI Timer
    RTC.PITCTRLA = RTC_PERIOD_CYC512_gc | RTC_PITEN_bm;
}

inline void mcu_wdt_standby() {
    RTC.PITINTCTRL = RTC_PI_bm;   // enable the Periodic Interrupt
    while (RTC.PITSTATUS > 0) {}  // make sure the register is ready to be updated
    // Set period (64 Hz / STANDBY_TICK_SPEED = 8 Hz), enable the PI Timer
    RTC.PITCTRLA = (1<<6) | (STANDBY_TICK_SPEED<<3) | RTC_PITEN_bm;
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
    // Enable, timeout 8ms
    _PROTECTED_WRITE(WDT.CTRLA, WDT_PERIOD_8CLK_gc);
    sei();
    wdt_reset();
    while (1) {}
}

inline void prevent_reboot_loop() {
    // prevent WDT from rebooting MCU again
    RSTCTRL.RSTFR &= ~(RSTCTRL_WDRF_bm);  // reset status flag
    wdt_disable();  // from avr/wdt.h
}

