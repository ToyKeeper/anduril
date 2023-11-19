// arch/avr32dd20.h: avr32dd20 support header
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

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
    // set left- or right-adjust (right)
    // set free-running mode or not (yes)
    ADC0.CTRLA = ADC_CONVMODE_SINGLEENDED_gc
               | ADC_RESSEL_12BIT_gc
               //| ADC_LEFTADJ_bm  // not in temperature mode
               | ADC_FREERUN_bm;
    // set number of samples (requires adjustment in formula too)
    ADC0.CTRLB = ADC_SAMPNUM_NONE_gc;
    // TODO: accumulate more samples for more resolution
    // (and probably set the prescale faster too)
    //ADC0.CTRLB = ADC_SAMPNUM_ACC16_gc;  // 16 samples per result
    // set a clock prescaler
    ADC0.CTRLC = ADC_PRESC_DIV64_gc;
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
    ADC0.CTRLC = ADC_PRESC_DIV16_gc;  // not too fast, not too slow
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
    // FIXME: enable this after getting ADC stuff fixed
    #if 0
    ADC0.INTCTRL |= ADC_RESRDY_bm; // enable interrupt
    ADC0.COMMAND |= ADC_STCONV_bm; // actually start measuring
    #endif
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

inline uint16_t mcu_adc_result_temp() {
    // FIXME: better math, higher precision
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
    // voltage is 12-bit right-aligned + 16x oversampling = 16 bits total
    return ADC0.RES;
}

inline uint8_t mcu_adc_lsb() {
    // temp is right-aligned, voltage is 16-bit, both have a useful LSB
    return ADC0_RESL;
}

#ifdef USE_VOLTAGE_VDD
uint8_t calc_voltage(uint16_t measurement) {
    // calculate actual voltage: volts * 10
    // FIXME
    // ADC = 1.1 * 1024 / volts
    // volts = 1.1 * 1024 / ADC
    result = ((uint16_t)(2*1.1*1024*10)/(measurement>>6)
              + VOLTAGE_FUDGE_FACTOR
              #ifdef USE_VOLTAGE_CORRECTION
                  + VOLT_CORR - 7
              #endif
              ) >> 1;
    return result;
}
#elif defined(USE_VOLTAGE_VDDIO2)
#elif defined(USE_VOLTAGE_VDDIO2)
#else
// hwdef must supply its own function
#endif

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

