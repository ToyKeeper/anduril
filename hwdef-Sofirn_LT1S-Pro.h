/* BLF LT1S Pro driver layout using the Attiny1616

Driver pinout:
 * eSwitch:    PA5
 * Aux LED:    PB5
 * WW PWM:     PB0 (TCA0 WO0)
 * CW PWM:     PB1 (TCA0 WO1)
 * Red PWM:    PB2 (TCA0 WO2)
 * Voltage:    VCC

*/

#pragma once


#define HWDEF_C_FILE hwdef-Sofirn_LT1S-Pro.c

#define ATTINY 1616
#include <avr/io.h>

// channel modes:
// * 0. warm/cool white blend
// * 1. auto 3ch blend (red -> warm -> cool by ramp level)
// * 2. red only
// * 3. red + white blend
#define NUM_CHANNEL_MODES 4
//#define CHANNEL_MODES_ENABLED 1,1,1,1
#define CHANNEL_MODES_ENABLED 0b00001111
#define CM_WHITE      0
#define CM_AUTO       1
#define CM_RED        2
#define CM_WHITE_RED  3

// TODO: blend mode should enable this automatically?
#define USE_CHANNEL_MODES
// TODO: blend mode should enable this automatically?
#define USE_CHANNEL_MODE_ARGS
// TODO: or maybe if args are defined, the USE_ should be auto-set?
// 128=middle CCT, N/A, N/A, 255=100% red
#define CHANNEL_MODE_ARGS 128,0,0,255
#define SET_LEVEL_MODES      set_level_2ch_dyn_blend, \
                             set_level_auto_3ch_dyn_blend, \
                             set_level_1ch_dyn, \
                             set_level_red_white_blend
// TODO: gradual ticking for thermal regulation
#define GRADUAL_TICK_MODES   gradual_tick_2ch_blend, \
                             gradual_tick_auto_3ch_blend, \
                             gradual_tick_1ch, \
                             gradual_tick_red_white_blend
// can use some of the common handlers?
//#define USE_SET_LEVEL_2CH_BLEND
//#define USE_SET_LEVEL_AUTO_3CH_BLEND
//#define USE_SET_LEVEL_1CH
//#define USE_SET_LEVEL_RED_WHITE_BLEND
// TODO:
//#define USE_GRADUAL_TICK_2CH_BLEND
//#define USE_GRADUAL_TICK_AUTO_3CH_BLEND
//#define USE_GRADUAL_TICK_1CH
//#define USE_GRADUAL_TICK_RED_WHITE_BLEND

#define DEFAULT_CHANNEL_MODE           CM_AUTO

#define FACTORY_RESET_WARN_CHANNEL     CM_RED
#define FACTORY_RESET_SUCCESS_CHANNEL  CM_WHITE

#define POLICE_COLOR_STROBE_CH1        CM_RED
#define POLICE_COLOR_STROBE_CH2        CM_WHITE

// TODO: remove this as soon as it's not needed
#define PWM_CHANNELS 1

#define SWITCH_PIN     PIN5_bp
#define SWITCH_PORT    VPORTA.IN
#define SWITCH_ISC_REG PORTA.PIN2CTRL
#define SWITCH_VECT    PORTA_PORT_vect
#define SWITCH_INTFLG  VPORTA.INTFLAGS


// dynamic PWM
// PWM parameters of all channels are tied together because they share a counter
#define PWM_TOP_INIT 511  // highest value used in the top half of the ramp
#define PWM_TOP TCA0.SINGLE.PERBUF   // holds the TOP value for for variable-resolution PWM
#define PWM_CNT TCA0.SINGLE.CNT   // for resetting phase after each TOP adjustment

// warm tint channel
//#define WARM_PWM_PIN PB0
#define WARM_PWM_LVL TCA0.SINGLE.CMP0BUF  // CMP1 is the output compare register for PB0

// cold tint channel
//#define COOL_PWM_PIN PB1
#define COOL_PWM_LVL TCA0.SINGLE.CMP1BUF  // CMP0 is the output compare register for PB1

// red channel
//#define RED_PWM_PIN PB2
#define RED_PWM_LVL TCA0.SINGLE.CMP2BUF   // CMP2 is the output compare register for PB2

// only using 16-bit PWM on this light
#define PWM_BITS 16
#define PWM_GET       PWM_GET16
#define PWM_DATATYPE  uint16_t
#define PWM1_DATATYPE uint16_t
#define PWM_DATATYPE2 uint32_t


// average drop across diode on this hardware
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif


// lighted button
#define AUXLED_PIN  PIN5_bp
#define AUXLED_PORT PORTB

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING


// custom channel modes
void set_level_1ch_dyn(uint8_t level);
void set_level_2ch_dyn_blend(uint8_t level);
void set_level_auto_3ch_dyn_blend(uint8_t level);
void set_level_red_white_blend(uint8_t level);


inline void hwdef_setup() {

    // set up the system clock to run at 10 MHz instead of the default 3.33 MHz
    _PROTECTED_WRITE( CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm );

    //VPORTA.DIR = ...;
    // Outputs:
    VPORTB.DIR = PIN0_bm   // warm white
               | PIN1_bm   // cool white
               | PIN2_bm   // red
               | PIN5_bm;  // aux LED
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

    //PORTB.PIN0CTRL = PORT_PULLUPEN_bm; // warm tint channel
    //PORTB.PIN1CTRL = PORT_PULLUPEN_bm; // cold tint channel
    //PORTB.PIN2CTRL = PORT_PULLUPEN_bm; // red LEDs
    PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
    //PORTB.PIN5CTRL = PORT_PULLUPEN_bm; // Aux LED

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
    // TODO: add references to MCU documentation
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm
                      | TCA_SINGLE_CMP1EN_bm
                      | TCA_SINGLE_CMP2EN_bm
                      | TCA_SINGLE_WGMODE_DSBOTTOM_gc;
    PWM_TOP = PWM_TOP_INIT;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
                      | TCA_SINGLE_ENABLE_bm;
}


#define LAYOUT_DEFINED

