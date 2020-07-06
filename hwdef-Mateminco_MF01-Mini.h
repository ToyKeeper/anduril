#ifndef HWDEF_MF01_MINI_H
#define HWDEF_MF01_MINI_H

/* MF01-Mini driver layout
 *           ----
 *   Reset -|1  8|- VCC
 * eswitch -|2  7|- aux LEDs
 * FET PWM -|3  6|- PWM (7x7135)
 *     GND -|4  5|- PWM (1x7135)
 *           ----
 */

#define PWM_CHANNELS 3

#ifndef AUXLED_PIN
#define AUXLED_PIN   PB2    // pin 7
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
#define PWM2_PIN PB1        // pin 6, 7x7135 PWM
#define PWM2_LVL OCR0B      // OCR0B is the output compare register for PB1
#endif
#ifndef PWM3_PIN
#define PWM3_PIN PB4        // pin 3, FET PWM
#define PWM3_LVL OCR1B      // OCR1B is the output compare register for PB4
#endif

#define ADC_PRSCL   0x07    // clk/128

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 5  // add 0.25V
#endif

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels

#define LAYOUT_DEFINED

#endif
