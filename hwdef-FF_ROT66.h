#ifndef HWDEF_FF_ROT66_H
#define HWDEF_FF_ROT66_H

/* Fireflies ROT66 driver layout
 * same as a FW3A, basically, except ...
 */

// ... except the ROT66 has a lighted button
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB2    // pin 7
#endif

// ... and slightly different calibration
#ifndef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
#endif

#include "hwdef-FW3A.h"

// ... and no optic nerve
#ifdef VISION_PIN
#undef VISION_PIN
#endif

#endif
