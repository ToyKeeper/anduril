/* Fireflies ROT66 driver layout
 */
// same as a FW3A, basically
#include "hwdef-FW3A.h"

// ... except the ROT66 has a lighted button
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB2    // pin 7
#endif

// ... and no optic nerve
#ifdef VISION_PIN
#undef VISION_PIN
#endif

// average drop across diode on this hardware
#ifdef VOLTAGE_FUDGE_FACTOR
#undef VOLTAGE_FUDGE_FACTOR
#endif
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
