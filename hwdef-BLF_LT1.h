#ifndef HWDEF_BLF_Q8_H
#define HWDEF_BLF_Q8_H

/* BLF Q8 driver layout
 * Q8 driver is the same as a D4, basically
 */

// ... except the Q8 has a lighted button
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB4    // pin 3
#endif

// ... and slightly different calibration
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

// Q8 driver is the same as a D4, basically
#include "hwdef-Emisar_D4.h"

#endif
