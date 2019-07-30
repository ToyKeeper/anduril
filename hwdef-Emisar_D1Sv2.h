#ifndef HWDEF_EMISAR_D1SV2_H
#define HWDEF_EMISAR_D1SV2_H

/* Emisar D1Sv2 driver layout (attiny1634)
 *
 * Pin / Name / Function
 *   1    PA6   red aux LED (PWM1B)
 *   2    PA5   green aux LED (PWM0B)
 *   3    PA4   blue aux LED
 *   4    PA3   battery voltage (ADC0)
 *   5    PA2   (none)
 *   6    PA1   (none)
 *   7    PA0   (none)
 *   8    GND   GND
 *   9    VCC   VCC
 *  10    PC5   (none)
 *  11    PC4   (none)
 *  12    PC3   RESET
 *  13    PC2   (none)
 *  14    PC1   SCK
 *  15    PC0   (none) PWM0A
 *  16    PB3   main LED PWM (PWM1A)
 *  17    PB2   MISO
 *  18    PB1   MOSI
 *  19    PB0   Opamp power
 *  20    PA7   e-switch  (PCINT7)
 *      ADC12   thermal sensor
 *
 * Main LED power uses one pin to turn the Opamp on/off,
 * and one pin to control Opamp power level.
 * All brightness control uses the power level pin, with 4 kHz 10-bit PWM.
 * The on/off pin is only used to turn the main LED on and off,
 * not to change brightness.
 */

#ifdef ATTINY
#undef ATTINY
#endif
#define ATTINY 1634
#include <avr/io.h>

#define PWM_CHANNELS 1
#define PWM_BITS 10  // 0 to 1023 at 4 kHz, not 0 to 255 at 16 kHz

#define SWITCH_PIN   PA7    // pin 20
#define SWITCH_PCINT PCINT7 // pin 20 pin change interrupt
#define SWITCH_PCIE  PCIE0  // PCIE0 is for PCINT[7:0]
#define SWITCH_PCMSK PCMSK0 // PCMSK0 is for PCINT[7:0]
#define SWITCH_PORT  PINA   // PINA or PINB or PINC

#define PWM1_PIN PB3        // pin 16, Opamp reference
#define PWM1_LVL OCR1A      // OCR1A is the output compare register for PB3

#define LED_ENABLE_PIN PB0  // pin 19, Opamp power
//#define PWM2_PIN PB0        // pin 19, Opamp power
// FIXME:
//#define PWM2_LVL OCR1B      // OCR1B is the output compare register for PB1


#define ADC_PRSCL   0x06    // clk/64

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 4  // add 0.20V  (measured 0.22V)
#endif

#define TEMP_CHANNEL 0b00001111

// this light has aux LEDs under the optic
#define AUXLED_R_PIN    PA6    // pin 1
#define AUXLED_G_PIN    PA5    // pin 2
#define AUXLED_B_PIN    PA4    // pin 3
#define AUXLED_RGB_PORT PORTA  // PORTA or PORTB or PORTC
#define AUXLED_RGB_DDR  DDRA   // DDRA or DDRB or DDRC
#define AUXLED_RGB_PUE  PUEA   // PUEA or PUEB or PUEC

// with so many pins, doing this all with #ifdefs gets awkward...
// ... so just hardcode it in each hwdef file instead
inline void hwdef_setup() {
  // enable output ports
  // Opamp level and Opamp on/off
  DDRB = (1 << PWM1_PIN)
       | (1 << LED_ENABLE_PIN);
  // aux R/G/B
  DDRA = (1 << AUXLED_R_PIN)
       | (1 << AUXLED_G_PIN)
       | (1 << AUXLED_B_PIN)
       ;

  // configure PWM
  // Setup PWM. F_pwm = F_clkio / 2 / N / TOP, where N = prescale factor, TOP = top of counter
  // pre-scale for timer: N = 1
  // WGM1[3:0]: 0,0,1,1: PWM, Phase Correct, 10-bit (DS table 12-5)
  // CS1[2:0]:    0,0,1: clk/1 (No prescaling) (DS table 12-6)
  // COM1A[1:0]:    1,0: PWM OC1A in the normal direction (DS table 12-4)
  // COM1B[1:0]:    0,0: PWM OC1B disabled (DS table 12-4)
  TCCR1A  = (1<<WGM11)  | (1<<WGM10)   // 10-bit (TOP=0x03FF) (DS table 12-5)
          | (1<<COM1A1) | (0<<COM1A0)  // PWM 1A in normal direction (DS table 12-4)
          | (0<<COM1B1) | (0<<COM1B0)  // PWM 1B in normal direction (DS table 12-4)
          ;
  TCCR1B  = (0<<CS12)   | (0<<CS11) | (1<<CS10)  // clk/1 (no prescaling) (DS table 12-6)
          | (0<<WGM13)  | (0<<WGM12)  // phase-correct PWM (DS table 12-5)
          ;

  // set up e-switch
  //PORTA = (1 << SWITCH_PIN);  // TODO: configure PORTA / PORTB / PORTC?
  PUEA = (1 << SWITCH_PIN);  // pull-up for e-switch
  SWITCH_PCMSK = (1 << SWITCH_PCINT);  // enable pin change interrupt
}

#define LAYOUT_DEFINED

#endif
