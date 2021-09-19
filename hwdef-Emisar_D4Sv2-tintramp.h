#ifndef HWDEF_D4SV2_TINTRAMP_H
#define HWDEF_D4SV2_TINTRAMP_H

/* Emisar D4Sv2 w/ tint ramping
 * (based on the Noctigon K9.3 driver layout (attiny1634))
 *
 * Pin / Name / Function
 *   1    PA6   2nd LED PWM (linear) (PWM1B)
 *   2    PA5   R: red aux LED (PWM0B)
 *   3    PA4   G: green aux LED
 *   4    PA3   B: blue aux LED
 *   5    PA2   button LED
 *   6    PA1   Opamp 2 enable (2nd LEDs)
 *   7    PA0   Opamp 1 enable (main LEDs)
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   (none)
 *  11    PC4   (none)
 *  12    PC3   RESET
 *  13    PC2   (none)
 *  14    PC1   SCK
 *  15    PC0   main LED PWM (FET) (PWM0A) (unused on some models because tint ramping)
 *  16    PB3   main LED PWM (linear) (PWM1A)
 *  17    PB2   MISO
 *  18    PB1   MOSI / battery voltage (ADC6)
 *  19    PB0   (none)
 *  20    PA7   e-switch (PCINT7)
 *      ADC12   thermal sensor
 *
 * Main LED power uses one pin to turn the Opamp on/off,
 * and one pin to control Opamp power level.
 * Main brightness control uses the power level pin, with 4 kHz 10-bit PWM.
 * The on/off pin is only used to turn the main LED on and off,
 * not to change brightness.
 */

#ifdef ATTINY
#undef ATTINY
#endif
#define ATTINY 1634
#include <avr/io.h>

#define PWM_CHANNELS 1  // 1 virtual channel (1 for main LEDs + 1 for 2nd LEDs)
#define PWM_BITS 14     // 0 to 16383 at variable Hz, not 0 to 255 at 16 kHz
#define PWM_TOP 511
// dynamic PWM with tint ramping
#define USE_DYN_PWM  // dynamic frequency and speed
#define PWM_DATATYPE2 uint32_t  // only needs 32-bit if ramp values go over 255

#define SWITCH_PIN   PA7     // pin 20
#define SWITCH_PCINT PCINT7  // pin 20 pin change interrupt
#define SWITCH_PCIE  PCIE0   // PCIE1 is for PCINT[7:0]
#define SWITCH_PCMSK PCMSK0  // PCMSK1 is for PCINT[7:0]
#define SWITCH_PORT  PINA    // PINA or PINB or PINC
#define PCINT_vect   PCINT0_vect  // ISR for PCINT[7:0]

// usually PWM1_LVL would be a hardware register, but we need to abstract
// it out to a soft brightness value, in order to handle tint ramping
// (this allows smooth thermal regulation to work, and makes things
//  otherwise simpler and easier)
uint16_t PWM1_LVL;
#define PWM1_PIN PB3        // pin 16, Opamp reference
#define TINT1_LVL OCR1A     // OCR1A is the output compare register for PB3
#define PWM1_CNT TCNT1      // for dynamic PWM, reset phase

// gah, this driver is weird...
// two linear channels are treated as one,
// while there's also a FET on one channel for turbo on half the LEDs
// so the FET needs to be "PWM2" but the second linear is "TINT2"
#define PWM3_PIN PA6        // pin 1, 2nd LED Opamp reference
#define TINT2_LVL OCR1B     // OCR1B is the output compare register for PA6

#define PWM2_PIN PC0        // pin 15, DD FET PWM
#define PWM2_LVL OCR0A      // OCR0A is the output compare register for PC0

// PWM parameters of both channels are tied together because they share a counter
#define PWM1_TOP ICR1       // holds the TOP value for for variable-resolution PWM

#define LED_ENABLE_PIN  PA0    // pin 7, Opamp power
#define LED_ENABLE_PORT PORTA  // control port for PA0

#define LED2_ENABLE_PIN  PA1    // pin 6, Opamp power
#define LED2_ENABLE_PORT PORTA  // control port for PA1


#define USE_VOLTAGE_DIVIDER  // use a dedicated pin, not VCC, because VCC input is flattened
#define VOLTAGE_PIN PB1      // Pin 18 / PB1 / ADC6
// pin to ADC mappings are in DS table 19-4
#define VOLTAGE_ADC      ADC6D  // digital input disable pin for PB1
// DIDR0/DIDR1 mappings are in DS section 19.13.5, 19.13.6
#define VOLTAGE_ADC_DIDR DIDR1  // DIDR channel for ADC6D
// DS tables 19-3, 19-4
// Bit   7     6     5      4     3    2    1    0
//     REFS1 REFS0 REFEN ADC0EN MUX3 MUX2 MUX1 MUX0
// MUX[3:0] = 0, 1, 1, 0 for ADC6 / PB1
// divided by ...
// REFS[1:0] = 1, 0 for internal 1.1V reference
// other bits reserved
#define ADMUX_VOLTAGE_DIVIDER 0b10000110
#define ADC_PRSCL   0x07    // clk/128

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
//   (voltage - D1) * (R2/(R2+R1) * 1024 / 1.1)
// D1, R1, R2 = 0, 330, 100
#ifndef ADC_44
//#define ADC_44 981  // raw value at 4.40V
#define ADC_44 967  // manually tweaked so 4.16V will blink out 4.2
#endif
#ifndef ADC_22
//#define ADC_22 489  // raw value at 2.20V
#define ADC_22 482  // manually tweaked so 4.16V will blink out 4.2
#endif

// this light has aux LEDs under the optic
#define AUXLED_R_PIN    PA5    // pin 2
#define AUXLED_G_PIN    PA4    // pin 3
#define AUXLED_B_PIN    PA3    // pin 4
#define AUXLED_RGB_PORT PORTA  // PORTA or PORTB or PORTC
#define AUXLED_RGB_DDR  DDRA   // DDRA or DDRB or DDRC
#define AUXLED_RGB_PUE  PUEA   // PUEA or PUEB or PUEC

#define BUTTON_LED_PIN  PA2    // pin 5
#define BUTTON_LED_PORT PORTA  // for all "PA" pins
#define BUTTON_LED_DDR  DDRA   // for all "PA" pins
#define BUTTON_LED_PUE  PUEA   // for all "PA" pins

// with so many pins, doing this all with #ifdefs gets awkward...
// ... so just hardcode it in each hwdef file instead
inline void hwdef_setup() {
  // enable output ports
  DDRC = (1 << PWM2_PIN);
  DDRB = (1 << PWM1_PIN);
  DDRA = (1 << PWM3_PIN)
       | (1 << AUXLED_R_PIN)
       | (1 << AUXLED_G_PIN)
       | (1 << AUXLED_B_PIN)
       | (1 << BUTTON_LED_PIN)
       | (1 << LED_ENABLE_PIN)
       | (1 << LED2_ENABLE_PIN)
       ;

  // configure PWM
  // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
  // pre-scale for timer: N = 1
  // Linear opamp PWM for both main and 2nd LEDs (10-bit)
  // WGM1[3:0]: 1,0,1,0: PWM, Phase Correct, adjustable (DS table 12-5)
  // CS1[2:0]:    0,0,1: clk/1 (No prescaling) (DS table 12-6)
  // COM1A[1:0]:    1,0: PWM OC1A in the normal direction (DS table 12-4)
  // COM1B[1:0]:    1,0: PWM OC1B in the normal direction (DS table 12-4)
  TCCR1A  = (1<<WGM11)  | (0<<WGM10)   // adjustable PWM (TOP=ICR1) (DS table 12-5)
          | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A in normal direction (DS table 12-4)
          | (1<<COM1B1) | (0<<COM1B0)  // PWM 1B in normal direction (DS table 12-4)
          ;
  TCCR1B  = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
          | (1<<WGM13)  | (0<<WGM12)  // phase-correct adjustable PWM (DS table 12-5)
          ;

  // FET PWM (8-bit; this channel can't do 10-bit)
  // WGM0[2:0]: 0,0,1: PWM, Phase Correct, 8-bit (DS table 11-8)
  // CS0[2:0]:  0,0,1: clk/1 (No prescaling) (DS table 11-9)
  // COM0A[1:0]:  1,0: PWM OC0A in the normal direction (DS table 11-4)
  // COM0B[1:0]:  1,0: PWM OC0B in the normal direction (DS table 11-7)
  TCCR0A  = (0<<WGM01)  | (1<<WGM00)   // 8-bit (TOP=0xFF) (DS table 11-8)
          | (1<<COM0A1) | (0<<COM0A0)  // PWM 0A in normal direction (DS table 11-4)
          //| (1<<COM0B1) | (0<<COM0B0)  // PWM 0B in normal direction (DS table 11-7)
          ;
  TCCR0B  = (0<<CS02)   | (0<<CS01) | (1<<CS00)  // clk/1 (no prescaling) (DS table 11-9)
          | (0<<WGM02)  // phase-correct PWM (DS table 11-8)
          ;
  // set PWM resolution
  PWM1_TOP = PWM_TOP;

  // set up e-switch
  PUEA = (1 << SWITCH_PIN);  // pull-up for e-switch
  SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}

#define LAYOUT_DEFINED

#endif
