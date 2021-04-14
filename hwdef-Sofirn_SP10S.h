#ifndef HWDEF_SOFIRN_SP10S_H
#define HWDEF_SOFIRN_SP10S_H

// TODO: rename to sofirn-sp10s-gchart?

/* gChart's PIC12 to ATTINY1616 v1 adapter for the SP10S
https://oshpark.com/shared_projects/b4IZEGSy

PIC12 Pinout:
1 - VDD
2 - No Connect
3 - Low Channel FET (series 4.7K Ohms)
4 - Switch
5 - High Channel FET (main PWM)
6 - Voltage Divider (300K:100K Ohms)
7 - Boost chip enable
8 - GND

ATTINY1616 Mapping:
2 - PA5 : (no connect)
3 - PB5 : TCA0 - WO2 Alternate MUX
4 - PB3 : (switch)
5 - PB0 : TCA0 - WO0
6 - PB4 : ADC0 - AIN9
7 - PA1 : (boost enable)

*/


#define LAYOUT_DEFINED

#ifdef ATTINY
#undef ATTINY
#endif
#define ATTINY 1616
#include <avr/io.h>

#ifndef SWITCH_PIN
#define SWITCH_PIN     3
#define SWITCH_PORT    VPORTB.IN
#define SWITCH_ISC_REG PORTB.PIN3CTRL
#define SWITCH_VECT    PORTB_PORT_vect
#define SWITCH_INTFLG  VPORTB.INTFLAGS
#endif

#define PWM_CHANNELS 2

// Small channel
#ifndef PWM1_PIN
#define PWM1_PIN PB5
#define PWM1_LVL TCA0.SINGLE.CMP2  // PB5 is Alternate MUX for TCA Compare 2
#endif

// Big channel
#ifndef PWM2_PIN
#define PWM2_PIN PB0
#define PWM2_LVL TCA0.SINGLE.CMP0  // PB0 is TCA Compare 0
#endif

#define LED_ENABLE_PIN   PIN1_bp
#define LED_ENABLE_PORT  PORTA_OUT

#define USE_VOLTAGE_DIVIDER       // use a dedicated pin, not VCC, because VCC input is flattened
#define DUAL_VOLTAGE_FLOOR    20  // for AA/14500 boost drivers, don't indicate low voltage if below this level
#define DUAL_VOLTAGE_LOW_LOW  07  // the lower voltage range's danger zone 0.7 volts
#define ADMUX_VOLTAGE_DIVIDER ADC_MUXPOS_AIN9_gc  // which ADC channel to read

// Raw ADC readings at 4.4V and 2.2V
// calibrate the voltage readout here
// estimated / calculated values are:
//   (voltage - D1) * (R2/(R2+R1) * 1024 / 1.1)
// Resistors are 300,000 and 100,000
#ifndef ADC_44
#define ADC_44 1023  // raw value at 4.40V
#endif
#ifndef ADC_22
#define ADC_22 512  // raw value at 2.20V
#endif



// with so many pins, doing this all with #ifdefs gets awkward...
// ... so just hardcode it in each hwdef file instead
inline void hwdef_setup() {

    // set up the system clock to run at 5 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm );

    VPORTA.DIR = PIN1_bm;  // Boost enable pin
    VPORTB.DIR = PIN0_bm | PIN5_bm;  // PWM pins as output
    //VPORTC.DIR = ...;

    // enable pullups on the input pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN1CTRL = PORT_PULLUPEN_bm; // Boost enable pin
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    //PORTB.PIN0CTRL = PORT_PULLUPEN_bm; // Big PWM channel
    PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN3CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;  // Switch
    //PORTB.PIN4CTRL = PORT_PULLUPEN_bm; // Voltage divider
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm; // Small PWM channel

    //PORTC.PIN0CTRL = PORT_PULLUPEN_bm; connected to the ADC via airwire
    PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;

    // set up the PWM
    // TODO: add references to MCU documentation
    PORTMUX.CTRLC = PORTMUX_TCA02_ALTERNATE_gc;  // Use alternate pin for TCA0:WO2
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA0.SINGLE.PER = 255;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
}


#endif
