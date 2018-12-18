#ifndef HWDEF_FF_PL47_H
#define HWDEF_FF_PL47_H

/* Fireflies PL47 driver layout
 * same as a D4S, basically, except ...
 */

// ... the PL47 has aux LEDs on pin 7
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB2    // pin 7
#endif

// ... and switch LEDs on pin 3
#ifndef AUXLED2_PIN
#define AUXLED2_PIN   PB4    // pin 3
#endif

// ... and slightly different calibration
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

#include "hwdef-Emisar_D4S.h"
#undef FSM_EMISAR_D4S_DRIVER
#undef FSM_EMISAR_D4_DRIVER

#endif
