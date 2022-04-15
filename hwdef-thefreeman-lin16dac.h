#ifndef HWDEF_THEFREEMAN_LIN18_H
#define HWDEF_THEFREEMAN_LIN18_H

/* thefreeman's Linear 16 driver using DAC control

PA6 - DAC for LED brightness control
PA7 - Op-amp enable pin
PB5 - Aux LED
PB4 - Switch pin, internal pullup
PB3 - HDR control, set High to enable the high power channel, set Low for low power
Read voltage from VCC pin, has PFET so no drop

*/


#define LAYOUT_DEFINED

#ifdef ATTINY
#undef ATTINY
#endif
#define ATTINY 1616
#include <avr/io.h>

#define PWM_CHANNELS 1
#define USE_DYN_PWM  // dynamic frequency and speed

#ifndef SWITCH_PIN
#define SWITCH_PIN     PIN4_bp
#define SWITCH_PORT    VPORTB.IN
#define SWITCH_ISC_REG PORTB.PIN2CTRL
#define SWITCH_VECT    PORTB_PORT_vect
#define SWITCH_INTFLG  VPORTB.INTFLAGS
#endif


#define PWM1_LVL DAC0.DATA  // use this for DAC voltage output

// PWM parameters of both channels are tied together because they share a counter
#define PWM1_TOP VREF.CTRLA   // holds the TOP value for for variable-resolution PWM

// For enabling / disabling the HDR high-range channel
#define LED_ENABLE_PIN   PIN3_bp
#define LED_ENABLE_PORT  PORTB_OUT
#define LED_ENABLE_PIN_LEVEL_MIN 35
#define LED_ENABLE_PIN_LEVEL_MAX 150

// For turning on and off the op-amp
#define LED2_ENABLE_PIN   PIN7_bp
#define LED2_ENABLE_PORT  PORTA_OUT
#define LED2_ON_DELAY 80  // how many ms to delay turning on the lights after enabling the channel

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 0  // using a PFET so no appreciable drop
#endif

// lighted button
#ifndef AUXLED_PIN
#define AUXLED_PIN  PIN5_bp
#define AUXLED_PORT PORTB
#endif


// with so many pins, doing this all with #ifdefs gets awkward...
// ... so just hardcode it in each hwdef file instead
inline void hwdef_setup() {

    // set up the system clock to run at 10 MHz instead of the default 3.33 MHz
    // TODO: for this DAC controlled-light, try to decrease the clock speed or use the ULP
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );

    VPORTA.DIR = PIN6_bm | PIN7_bm;
    VPORTB.DIR = PIN3_bm;
    //VPORTC.DIR = 0b00000000;

    // enable pullups on the input pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    //PORTA.PIN6CTRL = PORT_PULLUPEN_bm;  // DAC ouput
    //PORTA.PIN7CTRL = PORT_PULLUPEN_bm;  // Op-amp enable pin

    PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN2CTRL = PORT_PULLUPEN_bm; 
    //PORTB.PIN3CTRL = PORT_PULLUPEN_bm;  // HDR channel selection
    PORTB.PIN4CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;  // switch
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm;  // Aux LED

    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;

    // set up the DAC
    // https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf
    // DAC ranges from 0V to (255 * Vref) / 256
    VREF.CTRLA |= VREF_DAC0REFSEL_2V5_gc; // also VREF_DAC0REFSEL_0V55_gc and VREF_DAC0REFSEL_1V1_gc and VREF_DAC0REFSEL_2V5_gc 
    VREF.CTRLB |= VREF_DAC0REFEN_bm;
    DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm;
    DAC0.DATA = 255; // set the output voltage

}


#endif
