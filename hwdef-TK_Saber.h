#ifndef HWDEF_TK_SABER_H
#define HWDEF_TK_SABER_H

/* TK 4-color lightsaber driver layout
 *             ----
 *     Reset -|1  8|- VCC
 * PWM 4 (A) -|2  7|- e-switch
 * PWM 3 (G) -|3  6|- PWM 2 (B)
 *       GND -|4  5|- PWM 1 (R)
 *             ----
 */

#define PWM_CHANNELS 4
#define PWM1_PIN PB0        // pin 5
#define PWM1_LVL OCR0A
#define PWM2_PIN PB1        // pin 6
#define PWM2_LVL OCR0B
#define PWM3_PIN PB4        // pin 3
#define PWM3_LVL OCR1B
#define PWM4_PIN PB3        // pin 2
#define PWM4_LVL OCR1A

#define SWITCH_PIN   PB2    // pin 7
#define SWITCH_PCINT PCINT2 // pin 7 pin change interrupt

#define ADC_PRSCL   0x07    // clk/128

// average drop across diode on this hardware
#define VOLTAGE_FUDGE_FACTOR 5  // add 0.25V

#define FAST        0xA3    // fast PWM both channels
#define PHASE       0xA1    // phase-correct PWM both channels

#define LAYOUT_DEFINED

#endif
