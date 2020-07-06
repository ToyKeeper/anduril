#ifndef HWDEF_MF01S_H
#define HWDEF_MF01S_H

/* MF01S driver layout
 *           ----
 *   Reset -|1  8|- VCC (unused)
 * eswitch -|2  7|- Voltage divider (2S)
 * AUX LED -|3  6|- PWM (FET)
 *     GND -|4  5|- PWM (smaller FET)
 *           ----
 */

#define PWM_CHANNELS 2

#ifndef AUXLED_PIN
#define AUXLED_PIN   PB4    // pin 3
#endif

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

#define USE_VOLTAGE_DIVIDER // use a voltage divider on pin 7, not VCC
#ifndef VOLTAGE_PIN
#define VOLTAGE_PIN PB2     // pin 7, voltage ADC
#define VOLTAGE_CHANNEL 0x01 // MUX 01 corresponds with PB2
#define VOLTAGE_ADC ADC1D  // Digital input disable bit corresponding with PB2
// inherited from tk-attiny.h
//#define VOLTAGE_ADC_DIDR DIDR0  // DIDR for ADC1
// 1.1V reference, left-adjust, ADC1/PB2
//#define ADMUX_VOLTAGE_DIVIDER ((1 << V_REF) | (1 << ADLAR) | VOLTAGE_CHANNEL)
// 1.1V reference, no left-adjust, ADC1/PB2
#define ADMUX_VOLTAGE_DIVIDER ((1 << V_REF) | VOLTAGE_CHANNEL)
#endif
#define ADC_PRSCL   0x07    // clk/128

// Raw ADC readings at 4.4V and 2.2V (in-between, we assume values form a straight line)
#ifndef ADC_44
#define ADC_44 (234*4)
#endif
#ifndef ADC_22
#define ADC_22 (117*4)
#endif

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels

#define LAYOUT_DEFINED

#endif
