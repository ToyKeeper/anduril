#ifndef HWDEF_EMISAR_D4_H
#define HWDEF_EMISAR_D4_H

/* Emisar D4 driver layout
 *           ----
 *   Reset -|1  8|- VCC
 * eswitch -|2  7|-
 * AUX LED -|3  6|- PWM (FET)
 *     GND -|4  5|- PWM (1x7135)
 *           ----
 */

#define LAYOUT_DEFINED

#define PWM_CHANNELS 2

//#define AUXLED_PIN   PB4    // pin 3

#ifndef SWITCH_PIN
#define SWITCH_PIN   PB3    // pin 2
#define SWITCH_PCINT PCINT3 // pin 2 pin change interrupt
#endif

#ifndef PWM1_PIN
#define PWM1_PIN PB0        // pin 5, 1x7135 PWM
#define PWM1_LVL OCR0A      // OCR0A is the output compare register for PB0
#endif
#ifndef PWM2_PIN
#define PWM2_PIN PB1        // pin 6, FET PWM
#define PWM2_LVL OCR0B      // OCR0B is the output compare register for PB1
#endif

// (FIXME: remove?  not used?)
//#define VOLTAGE_PIN PB2     // pin 7, voltage ADC
//#define ADC_CHANNEL 0x01    // MUX 01 corresponds with PB2
//#define ADC_DIDR    ADC1D   // Digital input disable bit corresponding with PB2
#define ADC_PRSCL   0x07    // clk/128

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 5  // add 0.25V
#endif

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels

#endif
