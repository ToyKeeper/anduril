#ifndef HWDEF_BLF_LANTERN_T1616_H
#define HWDEF_BLF_LANTERN_T1616_H

/* BLF LT1 driver layout using the Attiny1616

Driver pinout:
 * eSwitch:    PA5
 * Aux LED:    PB5
 * PWM FET:    PB0 (TCA0 WO0)
 * PWM 1x7135: PB1 (TCA0 WO1)
 * Voltage:    VCC

*/


#define LAYOUT_DEFINED

#ifdef ATTINY
#undef ATTINY
#endif
#define ATTINY 1616
#include <avr/io.h>

#define PWM_CHANNELS 1

#ifndef SWITCH_PIN
#define SWITCH_PIN     PIN5_bp
#define SWITCH_PORT    VPORTA.IN
#define SWITCH_ISC_REG PORTA.PIN2CTRL
#define SWITCH_VECT    PORTA_PORT_vect
#define SWITCH_INTFLG  VPORTA.INTFLAGS
#endif


// usually PWM1_LVL would be a hardware register, but we need to abstract
// it out to a soft brightness value, in order to handle tint ramping
// (this allows smooth thermal regulation to work, and makes things
//  otherwise simpler and easier)
uint8_t PWM1_LVL;

// warm tint channel
#ifndef PWM1_PIN
#define PWM1_PIN PB1                //
#define TINT1_LVL TCA0.SINGLE.CMP1  // CMP1 is the output compare register for PB1
#endif

// cold tint channel
#ifndef PWM2_PIN
#define PWM2_PIN PB0                //
#define TINT2_LVL TCA0.SINGLE.CMP0  // CMP0 is the output compare register for PB0
#endif

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif


// lighted button
#ifndef AUXLED_PIN
#define AUXLED_PIN  PIN5_bp
#define AUXLED_PORT PORTB
#endif


// with so many pins, doing this all with #ifdefs gets awkward...
// ... so just hardcode it in each hwdef file instead
inline void hwdef_setup() {

    // set up the system clock to run at 5 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm );

    //VPORTA.DIR = ...;
    VPORTB.DIR = PIN0_bm | PIN1_bm | PIN5_bm;  // Outputs: Aux LED and PWMs
    //VPORTC.DIR = ...;

    // enable pullups on the unused pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;  // eSwitch
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    //PORTB.PIN0CTRL = PORT_PULLUPEN_bm; // cold tint channel
    //PORTB.PIN1CTRL = PORT_PULLUPEN_bm; // warm tint channel
    PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm; // Aux LED

    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;

    // set up the PWM
    // TODO: add references to MCU documentation
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA0.SINGLE.PER = 255;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
}


#endif
