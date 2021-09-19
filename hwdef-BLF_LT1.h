#ifndef HWDEF_BLF_LT1_H
#define HWDEF_BLF_LT1_H

/* BLF LT1 driver layout
 *           ----
 *   Reset -|1  8|- VCC
 * eswitch -|2  7|- (unused)
 * aux LED -|3  6|- PWM (5000K)
 *     GND -|4  5|- PWM (3000K)
 *           ----
 */

#define ATTINY 85
#include <avr/io.h>

#define PWM_CHANNELS 1  // 1 virtual channel (1 for main LEDs + 1 for 2nd LEDs)
#define PWM_BITS 8      // 0 to 255 at 15.6 kHz
#define PWM_TOP 255

// dynamic PWM with tint ramping (not supported on attiny85)
//#define USE_DYN_PWM     // dynamic frequency and speed
//#define PWM1_CNT TCNT0  // for dynamic PWM, reset phase

// usually PWM1_LVL would be a hardware register, but we need to abstract
// it out to a soft brightness value, in order to handle tint ramping
// (this allows smooth thermal regulation to work, and makes things
//  otherwise simpler and easier)
uint8_t PWM1_LVL;

#define PWM1_PIN PB0        // pin 5, warm tint PWM
#define TINT1_LVL OCR0A     // OCR0A is the output compare register for PB0

#define PWM2_PIN PB1        // pin 6, cold tint PWM
#define TINT2_LVL OCR0B     // OCR0B is the output compare register for PB1


#define AUXLED_PIN   PB4    // pin 3

#define SWITCH_PIN   PB3    // pin 2
#define SWITCH_PCINT PCINT3 // pin 2 pin change interrupt

#define ADC_PRSCL   0x07    // clk/128

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

#define FAST 0xA3           // fast PWM both channels
#define PHASE 0xA1          // phase-correct PWM both channels

#define LAYOUT_DEFINED


#endif
