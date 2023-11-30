// FW3X Lume1 helper functions
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fsm/chan-rgbaux.c"


void set_level_zero();

void set_level_main(uint8_t level);
bool gradual_tick_main(uint8_t gt);


Channel channels[] = {
    { // channel 1 only
        .set_level    = set_level_main,
        .gradual_tick = gradual_tick_main
    },
    RGB_AUX_CHANNELS
};


void set_level_zero() {
    // disable timer overflow interrupt
    // (helps improve button press handling from Off state)
    DSM_INTCTRL &= ~DSM_OVF_bm;

    // turn off all LEDs
    ch1_dsm_lvl = 0;
    CH1_PWM = 0;
    CH2_PWM = 0;
    PWM_CNT = 0;  // reset phase
    CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable regulator
}

// single set of LEDs with 2 stacked power channels, regulated + DD FET
void set_level_main(uint8_t level) {
    if (level == actual_level - 1) return;  // prevent flicker on no-op

    PWM1_DATATYPE ch1 = PWM1_GET(level);
    PWM2_DATATYPE ch2 = PWM2_GET(level);

    // set delta-sigma soft levels
    ch1_dsm_lvl = ch1;

    // set hardware PWM levels and init dsm loop
    CH1_PWM = ch1_pwm = ch1 >> 7;
    CH2_PWM =           ch2;

    // enable timer overflow interrupt so DSM can work
    DSM_INTCTRL |= DSM_OVF_bm;

    // force reset phase when turning on from zero
    // (for faster, more consistent initial response)
    if (! actual_level) PWM_CNT = 0;

    // don't enable ch1 and ch2 at the same time
    if (ch2) CH1_ENABLE_PORT &= ~(1 << CH1_ENABLE_PIN);  // disable regulator
    else CH1_ENABLE_PORT |= (1 << CH1_ENABLE_PIN);  // enable regulator
}

// delta-sigma modulation of PWM outputs
// happens on each Timer overflow (every 512 cpu clock cycles)
// uses 8-bit pwm w/ 7-bit dsm (0b 0PPP PPPP PDDD DDDD)
ISR(DSM_vect) {
    // set new hardware values first,
    // for best timing (reduce effect of interrupt jitter)
    CH1_PWM = ch1_pwm;

    // calculate next values, now that timing matters less

    // accumulate error
    ch1_dsm += (ch1_dsm_lvl & 0x007f);
    // next PWM = base PWM value + carry bit
    ch1_pwm  = (ch1_dsm_lvl >> 7) + (ch1_dsm > 0x7f);
    // clear carry bit
    ch1_dsm &= 0x7f;
}


bool gradual_tick_main(uint8_t gt) {
    // 150/150 is full FET + zero regulated,
    // 149/150 is zero FET + full regulated,
    // so don't try to gradually adjust between
    // if target is in the top 2 levels, just let the parent handle it
    if (gt >= RAMP_SIZE-2) return true;

    PWM1_DATATYPE ch1 = PWM1_GET(gt);

    // adjust multiple times based on current brightness
    // (so it adjusts faster/coarser when bright, slower/finer when dim)

    // higher shift = slower/finer adjustments
    const uint8_t shift = 9;  // ((255 << 7) >> 9) = 63 max
    uint8_t steps;

    steps = ch1_dsm_lvl >> shift;
    for (uint8_t i=0; i<=steps; i++)
        GRADUAL_ADJUST_SIMPLE(ch1, ch1_dsm_lvl);

    if ((ch1 == ch1_dsm_lvl)
       ) {
        return true;  // done
    }
    return false;  // not done yet
}

////////// external temperature sensor //////////

#ifdef ADMUX_THERM_EXTERNAL_SENSOR

void hwdef_set_admux_therm() {
    // put the ADC in temperature mode
    // ADCSRB: [VDEN, VDPD, -, -, ADLAR, ADTS2, ADTS1, ADTS0]
    ADCSRB = (1 << ADLAR);  // left-adjust, free-running
    // DS table 19-3, 19-4
    // [refs1, refs0, refen, adc0en, mux3, mux2, mux1, mux0]
    // refs=0b00 : VCC (2.5V)
    // mux=0b1011 : ADC11 (pin PC2)
    ADMUX = ADMUX_THERM_EXTERNAL_SENSOR;
    // other stuff is already set, so no need to re-set it
}

uint16_t temp_raw2cooked(uint16_t measurement) {
    // In: (raw ADC average) << 6
    // Out: Kelvin << 6
    /* notes from old versions of the code...
    // Used for Lume1 Driver: MCP9700 - T_Celsius = 100*(VOUT - 0.5V)
    // ADC is 2.5V reference, 0 to 1023
    // due to floating point, this calculation takes 916 extra bytes
    // (should use an integer equivalent instead)
    //#define EXTERN_TEMP_FORMULA(m) (((m)-205)/4.09)
    //int16_t celsius = (((measurement-205)/4.09)) + THERM_CAL_OFFSET + (int16_t)therm_cal_offset;
    */

    // this formula could probably be simplified... but meh, it works
    uint16_t k6 = ((uint32_t)(measurement - (205 << 6)) * 100 / 409)
                  + (273 << 6);  // convert back from C to K
    return k6;
}

#endif

