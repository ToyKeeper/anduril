#ifndef HWDEF_MT35_MINI_H
#define HWDEF_MT35_MINI_H

/* Mateminco MT35-Mini / Astrolux FT03
 *           ----
 *   Reset -|1  8|- VCC
 * eswitch -|2  7|- Aux LED
 *  1x7135 -|3  6|- NC
 *     GND -|4  5|- FET
 *           ----
 */

#define PWM_CHANNELS 2

#ifndef SWITCH_PIN
#define SWITCH_PIN   PB3    // pin 2
#define SWITCH_PCINT PCINT3 // pin 2 pin change interrupt
#endif

#ifndef PWM1_PIN
#define PWM1_PIN PB4        // pin 3, 1x7135 PWM
#define PWM1_LVL OCR1B      // OCR1B is the output compare register for PB0
#endif

#ifndef PWM2_PIN
#define PWM2_PIN PB0        // pin 5, FET PWM
#define PWM2_LVL OCR0A      // OCR0A is the output compare register for PB4
#endif

#define ADC_PRSCL   0x07    // clk/128

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

// lighted button
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB2    // pin 7
#endif

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels

#define LAYOUT_DEFINED

#endif
