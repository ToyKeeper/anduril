/* BLF Q8 driver layout
 */
// Q8 driver is the same as a D4, basically
#include "hwdef-Emisar_D4.h"

// ... except the Q8 has a lighted button
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB4    // pin 3
#endif

// average drop across diode on this hardware
#ifdef VOLTAGE_FUDGE_FACTOR
#undef VOLTAGE_FUDGE_FACTOR
#endif
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V

