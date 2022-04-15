#ifndef HWDEF_SOFIRN_SP10_H
#define HWDEF_SOFIRN_SP10_H

/* Sofirn SP10 Pro pinout

ATTINY1616 Mapping:
PB5 : PWM small channel (TCA0 - WO2 Alternate MUX)
PB3 : eSwitch
PB0 : PWM big channel (TCA0 - WO0)
PB4 : Voltage divider (ADC0 - AIN9)
PA1 : Boost Enable

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
#define PWM_BITS 16  // data type needs 16 bits, not 8
#define PWM_TOP  255 // highest value used in top half of ramp
#define USE_DYN_PWM  // dynamic frequency and speed

// Small channel
#ifndef PWM1_PIN
#define PWM1_PIN PB5
#define PWM1_LVL TCA0.SINGLE.CMP2BUF  // PB5 is Alternate MUX for TCA Compare 2
#endif

// Big channel
#ifndef PWM2_PIN
#define PWM2_PIN PB0
#define PWM2_LVL TCA0.SINGLE.CMP0BUF  // PB0 is TCA Compare 0
#endif

// PWM parameters of both channels are tied together because they share a counter
#define PWM1_TOP TCA0.SINGLE.PERBUF   // holds the TOP value for for variable-resolution PWM
// not necessary when double-buffered "BUF" registers are used
#define PWM1_CNT TCA0.SINGLE.CNT   // for resetting phase after each TOP adjustment
#define PWM1_PHASE_RESET_OFF  // force reset while shutting off
#define PWM1_PHASE_RESET_ON   // force reset while turning on
//#define PWM1_PHASE_SYNC       // manual sync while changing level

#define LED_ENABLE_PIN   PIN1_bp
#define LED_ENABLE_PORT  PORTA_OUT
//#define LED_OFF_DELAY 4  // only needed when PWM1_PHASE_RESET_OFF not used

#define USE_VOLTAGE_DIVIDER       // use a dedicated pin, not VCC, because VCC input is flattened
#define DUAL_VOLTAGE_FLOOR    21  // for AA/14500 boost drivers, don't indicate low voltage if below this level
#define DUAL_VOLTAGE_LOW_LOW   7  // the lower voltage range's danger zone 0.7 volts (NiMH)
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

    // set up the system clock to run at 10 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );

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

    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;

    // set up the PWM
    // https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf
    // PB0 is TCA0:WO0, use TCA_SINGLE_CMP0EN_bm
    // PB1 is TCA0:WO1, use TCA_SINGLE_CMP1EN_bm
    // PB2 is TCA0:WO2, use TCA_SINGLE_CMP2EN_bm
    // For Fast (Single Slope) PWM use TCA_SINGLE_WGMODE_SINGLESLOPE_gc
    // For Phase Correct (Dual Slope) PWM use TCA_SINGLE_WGMODE_DSBOTTOM_gc
    // See the manual for other pins, clocks, configs, portmux, etc
    PORTMUX.CTRLC = PORTMUX_TCA02_ALTERNATE_gc;  // Use alternate pin for TCA0:WO2
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_DSBOTTOM_gc;
    PWM1_TOP = PWM_TOP;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
}


// set fuses, these carry over to the ELF file
// we need this for enabling BOD in Active Mode from the factory.
// settings can be verified / dumped from the ELF file using this
// command: avr-objdump -d -S -j .fuse anduril.elf
FUSES = {
    .WDTCFG  = FUSE_WDTCFG_DEFAULT,   // Watchdog Configuration
    .BODCFG  = FUSE_ACTIVE0_bm,       // BOD Configuration
    .OSCCFG  = FUSE_OSCCFG_DEFAULT,   // Oscillator Configuration
    .TCD0CFG = FUSE_TCD0CFG_DEFAULT,  // TCD0 Configuration
    .SYSCFG0 = FUSE_SYSCFG0_DEFAULT,  // System Configuration 0
    .SYSCFG1 = FUSE_SYSCFG1_DEFAULT,  // System Configuration 1
    .APPEND  = FUSE_APPEND_DEFAULT,   // Application Code Section End
    .BOOTEND = FUSE_BOOTEND_DEFAULT,  // Boot Section End
};


#endif
