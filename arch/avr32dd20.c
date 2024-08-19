// arch/avr32dd20.h: avr32dd20 support functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "arch/avr32dd20.h"

////////// clock speed / delay stuff //////////

inline void mcu_clock_speed() {
    // TODO: allow hwdef to define a base clock speed
    // run the internal clock at 12 MHz, not the default 4 MHz
    _PROTECTED_WRITE( CLKCTRL.OSCHFCTRLA,
                      CLKCTRL_FRQSEL_12M_gc | CLKCTRL_AUTOTUNE_bm );
    // (another option is to use 20 MHz / 2, like attiny1616 does)
    // divide 20 MHz to run at 10 MHz to match attiny1616
    //_PROTECTED_WRITE( CLKCTRL.MCLKCTRLB,
    //                  CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );
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

// ADC0.CTRLA bits:
//   RUNSTDBY, -, CONVMODE, LEFTADJ, RESSEL[1:0], FREERUN, ENABLE
// CTRLB bits:  -, -, -, -, -, SAMPNUM[2:0]
// CTRLC bits:  -, -, -, -, PRESC[3:0]
// CTRLD bits:  INITDLY[2:0], -, SAMPDLY[3:0]
// CTRLE bits:  -, -, -, -, -, WINCM[2:0]
// SAMPCTRL: 8 bits
// MUXPOS, MUXNEG: 7 bits each
// COMMAND:  -, -, -, -, -, -, SPCONV, STCONV

inline void mcu_set_admux_therm() {
    // ADC init: Datasheet section 33.3.2
    // Temperature mode: Datasheet section 33.3.3.8
    // measurements should be 12-bit right-adjusted single-ended conversion
    // set Vref to 2.048V for temperature mode
    VREF.ADC0REF = VREF_REFSEL_2V048_gc;
    // set temp sensor as input
    ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;
    // configure init delay to >= 25 us * Fclk_adc (no sample delay needed)
    ADC0.CTRLD = ADC_INITDLY_DLY32_gc | ADC_SAMPDLY_DLY0_gc;
    // configure ADC sample length to >= 28 us * Fclk_adc
    ADC0.SAMPCTRL = 32;
    // set single-ended or differential
    // set resolution to 12 bits
    // set left- or right-adjust
    // set free-running mode or not (yes)
    ADC0.CTRLA = ADC_CONVMODE_SINGLEENDED_gc
               | ADC_RESSEL_12BIT_gc
               | ADC_LEFTADJ_bm
               | ADC_FREERUN_bm;
    // set number of samples (requires adjustment in formula too)
    ADC0.CTRLB = ADC_SAMPNUM_NONE_gc;
    // accumulate more samples for more resolution
    ADC0.CTRLB = ADC_SAMPNUM_ACC16_gc;  // 16 samples per result
    // set a clock prescaler
    //ADC0.CTRLC = ADC_PRESC_DIV64_gc;  // use this when no accumulation
    ADC0.CTRLC = ADC_PRESC_DIV4_gc;  // measure faster when oversampling
    // enable the ADC
    ADC0.CTRLA |= ADC_ENABLE_bm;
    // actually start measuring (happens in another function)
    //ADC0.COMMAND |= ADC_STCONV_bm;
    // for each measurement:
    // process according to sigrow data + formula
}

inline void mcu_set_admux_voltage() {
    // ADC init: Datasheet section 33.3.2
    // set Vref
    VREF.ADC0REF = VREF_REFSEL_1V024_gc;
    // set single-ended or differential
    // set resolution to 12 bits
    // set left- or right-adjust (right)
    // set free-running mode or not (yes)
    ADC0.CTRLA = ADC_CONVMODE_SINGLEENDED_gc
               | ADC_RESSEL_12BIT_gc
               | ADC_LEFTADJ_bm  // has no effect when 16+ samples taken
               | ADC_FREERUN_bm
               | ADC_RUNSTBY_bm  // allow voltage sense in standby mode
               ;
    // set number of samples
    ADC0.CTRLB = ADC_SAMPNUM_ACC16_gc;  // 16 samples per result
    // set a clock prescaler
    ADC0.CTRLC = ADC_PRESC_DIV4_gc;  // measure faster when oversampling
    // select the positive ADC input with MUXPOS
    #ifdef USE_VOLTAGE_DIVIDER  // external voltage divider
        // ADC input pin / Vref
        ADC0.MUXPOS = ADMUX_VOLTAGE_DIVIDER;  // external pin
    #elif defined (USE_VOLTAGE_VDDIO2)  // internal voltage divider
        // (Vbat / 10) / Vref
        ADC0.MUXPOS = ADC_MUXPOS_VDDIO2DIV10_gc;
    #else  // measure directly on VDD/VCC pin
        // (Vbat / 10) / Vref
        ADC0.MUXPOS = ADC_MUXPOS_VDDDIV10_gc;
    #endif
    // enable the ADC
    ADC0.CTRLA |= ADC_ENABLE_bm;
    // actually start measuring (happens in another function)
    //ADC0.COMMAND |= ADC_STCONV_bm;
}

inline void mcu_adc_sleep_mode() {
    set_sleep_mode(SLEEP_MODE_STANDBY);
}

inline void mcu_adc_start_measurement() {
    ADC0.INTCTRL |= ADC_RESRDY_bm; // enable interrupt
    ADC0.COMMAND |= ADC_STCONV_bm; // actually start measuring
}

/*
void mcu_adc_on() {
    hwdef_set_admux_voltage();
    mcu_adc_start_measurement();
}
*/

inline void mcu_adc_off() {
    ADC0.CTRLA &= ~(ADC_ENABLE_bm);  // disable the ADC
}

inline void mcu_adc_vect_clear() {
    ADC0.INTFLAGS = ADC_RESRDY_bm; // clear the interrupt
}

inline uint16_t mcu_adc_result() {
    // value is 12-bit left-aligned + 16x oversampling = 16 bits total
    return ADC0.RES;
}

inline uint8_t mcu_vdd_raw2cooked(uint16_t measurement) {
    // In : 65535 * (Vbat / 10) / 1.024V
    // Out: uint8_t: Vbat * 50
    // (add 80 to round up near a boundary)
    uint8_t vbat50 = (uint16_t)(measurement + 64) / 128;
    return vbat50;
}

#if 0
inline uint16_t mcu_vdd_raw2fine(uint16_t measurement) {
    // In : 65535 * (Vbat / 10) / 1.024V
    // Out: 65535 * (Vbat / 10) / 1.024V
    // This MCU's native format is already correct
    return measurement;
}
#endif

inline uint16_t mcu_temp_raw2cooked(uint16_t measurement) {
    // convert raw ADC values to calibrated temperature
    // In: ADC raw temperature (16-bit, or 12-bit left-aligned)
    // Out: Kelvin << 6
    // Precision: 1/64th Kelvin (but noisy)
    // AVR DD datasheet section 33.3.3.8
    uint16_t sigrow_slope  = SIGROW.TEMPSENSE0;  // factory calibration data
    uint16_t sigrow_offset = SIGROW.TEMPSENSE1;  // 12-bit value
    //const uint32_t scaling_factor = 4096;  // use top 12 bits of ADC data
    //uint32_t temp = sigrow_offset - (measurement >> 4);
    const uint32_t scaling_factor = 65536;  // use all 16 bits of ADC data
    uint32_t temp = (sigrow_offset << 4) - measurement;
    temp *= sigrow_slope;  // 24-bit result
    temp += scaling_factor / 8;  // Add 1/8th K to get correct rounding on later divisions
    //temp = temp >>  6;  // change (K << 12) to (K << 6)
    temp = temp >> 10;  // change (K << 16) to (K << 6)
    return temp;  // left-aligned uint16_t, 0 to 1023.98 Kelvin
}

inline uint8_t mcu_adc_lsb() {
    // volts and temp are both 16-bit, so the LSB is useful as-is
    return ADC0_RESL;
}


////////// WDT //////////
// this uses the RTC PIT interrupt instead of WDT,
// as recommended on AVR 0/1-series and later:
// https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/PowerSave.md#using-the-real-time-counter-rtcpit
// The PIT runs even in power-down mode, unlike RTC,
// and its cycles are relative to the AVR's internal 32768 Hz ULP oscillator
// AVR datasheet sections 26, 26.5, 26.9, 26.12, 26.13.12

// PIT tick speeds:
//  0   (none)
//  1  8192 Hz (CYC4)
//  2  4096 Hz
//  3  2048 Hz
//  4  1024 Hz (CYC32)
//  5   512 Hz
//  6   256 Hz
//  7   128 Hz
//  8    64 Hz (default) (CYC512)
inline void mcu_wdt_active() {
    RTC.PITINTCTRL = RTC_PI_bm;   // enable the Periodic Interrupt
    while (RTC.PITSTATUS > 0) {}  // make sure the register is ready to be updated
    // Period = 16ms (64 Hz), enable the PI Timer
    RTC.PITCTRLA = RTC_PERIOD_CYC512_gc | RTC_PITEN_bm;
}

// STANDBY_TICK_SPEED:
//  0   64 Hz
//  1   32 Hz
//  2   16 Hz
//  3    8 Hz (default)
//  4    4 Hz
//  5    2 Hz
//  6    1 Hz
inline void mcu_wdt_standby() {
    RTC.PITINTCTRL = RTC_PI_bm;   // enable the Periodic Interrupt
    while (RTC.PITSTATUS > 0) {}  // make sure the register is ready to be updated
    // Set period (64 Hz / STANDBY_TICK_SPEED = 8 Hz), enable the PI Timer
    RTC.PITCTRLA = ((8+STANDBY_TICK_SPEED)<<3) | RTC_PITEN_bm;
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
    // request a reboot (software reset)
    _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRST_bm);
}

inline void prevent_reboot_loop() {
    // if previous reset was a crash (WDT time-out),
    // prevent it from happening again immediately
    //RSTCTRL.RSTFR &= ~(RSTCTRL_WDRF_bm);  // reset wdt flag only
    RSTCTRL.RSTFR = 0;  // reset all reset status flags (maybe unneeded?)
    wdt_disable();  // from avr/wdt.h
}

