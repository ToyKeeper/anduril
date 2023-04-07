#ifndef HWDEF_WURKKOS_TS25_T1616_H
#define HWDEF_WURKKOS_TS25_T1616_H

/* BLF Q8 driver layout using the Attiny1616

Driver pinout:
 * eSwitch:    PA5
 * PWM FET:    PB0 (TCA0 WO0)
 * PWM 1x7135: PB1 (TCA0 WO1)
 * Voltage:    VCC
 * Aux Blue:   PC1
 * Aux Red:    PC2
 * Aux Green:  PC3

*/


#define LAYOUT_DEFINED

#ifdef ATTINY
#undef ATTINY
#endif
#define ATTINY 1616
#include <avr/io.h>

#define PWM_CHANNELS 2
#define PWM_BITS 16
#define PWM_TOP 255
#define USE_DYN_PWM

#ifndef SWITCH_PIN
#define SWITCH_PIN     PIN5_bp
#define SWITCH_PORT    VPORTA.IN
#define SWITCH_ISC_REG PORTA.PIN2CTRL
#define SWITCH_VECT    PORTA_PORT_vect
#define SWITCH_INTFLG  VPORTA.INTFLAGS
#endif


// 7135 channel
#ifndef PWM1_PIN
#define PWM1_PIN PB1               //
#define PWM1_LVL TCA0.SINGLE.CMP1BUF  // CMP1 is the output compare register for PB1
#endif

// FET channel
#ifndef PWM2_PIN
#define PWM2_PIN PB0               //
#define PWM2_LVL TCA0.SINGLE.CMP0BUF  // CMP0 is the output compare register for PB0
#endif

// PWM parameters of both channels are tied together because they share a counter
#define PWM1_TOP TCA0.SINGLE.PERBUF   // holds the TOP value for for variable-resolution PWM
// not necessary when double-buffered "BUF" registers are used
#define PWM1_CNT TCA0.SINGLE.CNT   // for resetting phase after each TOP adjustment
#define PWM1_PHASE_RESET_OFF  // force reset while shutting off
#define PWM1_PHASE_RESET_ON   // force reset while turning on

// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif


// this driver allows for aux LEDs under the optic
#define AUXLED_B_PIN    PIN1_bp    // pin 1
#define AUXLED_R_PIN    PIN2_bp    // pin 2
#define AUXLED_G_PIN    PIN3_bp    // pin 3
#define AUXLED_RGB_PORT PORTC  // PORTA or PORTB or PORTC


// with so many pins, doing this all with #ifdefs gets awkward...
// ... so just hardcode it in each hwdef file instead
inline void hwdef_setup() {

    // set up the system clock to run at 10 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );

    //VPORTA.DIR = ...;
    VPORTB.DIR = PIN0_bm | PIN1_bm;  // Outputs: PWMs
    VPORTC.DIR = PIN1_bm | PIN2_bm | PIN3_bm;

    // enable pullups on the unused pins to reduce power
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;  // eSwitch
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    //PORTB.PIN0CTRL = PORT_PULLUPEN_bm; // FET channel
    //PORTB.PIN1CTRL = PORT_PULLUPEN_bm; // 7135 channel
    PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN5CTRL = PORT_PULLUPEN_bm;

    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
    //PORTC.PIN1CTRL = PORT_PULLUPEN_bm; // RGB Aux
    //PORTC.PIN2CTRL = PORT_PULLUPEN_bm; // RGB Aux
    //PORTC.PIN3CTRL = PORT_PULLUPEN_bm; // RGB Aux

    // set up the PWM
    // https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf
    // PB0 is TCA0:WO0, use TCA_SINGLE_CMP0EN_bm
    // PB1 is TCA0:WO1, use TCA_SINGLE_CMP1EN_bm
    // PB2 is TCA0:WO2, use TCA_SINGLE_CMP2EN_bm
    // For Fast (Single Slope) PWM use TCA_SINGLE_WGMODE_SINGLESLOPE_gc
    // For Phase Correct (Dual Slope) PWM use TCA_SINGLE_WGMODE_DSBOTTOM_gc
    // See the manual for other pins, clocks, configs, portmux, etc
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_DSBOTTOM_gc;
    PWM1_TOP = PWM_TOP;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
}


#endif
