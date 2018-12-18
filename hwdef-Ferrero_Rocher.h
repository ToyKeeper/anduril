#ifndef HWDEF_FERRERO_ROCHER_H
#define HWDEF_FERRERO_ROCHER_H

/* Ferrero Rocher driver layout
 *            ----
 *    Reset -|1  8|- VCC
 * E-switch -|2  7|- Voltage ADC
 *  Red LED -|3  6|- PWM
 *      GND -|4  5|- Green LED
 *            ----
 */
// TODO: fill in this section, update Ferrero_Rocher code to use it.
#define FAST 0x23           // fast PWM channel 1 only
#define PHASE 0x21          // phase-correct PWM channel 1 only

#define LAYOUT_DEFINED

#endif
