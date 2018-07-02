// Emisar D1S config options for Anduril
#include "cfg-emisar-d4.h"

// stop panicking at ~90% power or ~1200 lm (D1S has a good power-to-thermal-mass ratio)
#undef THERM_FASTER_LEVEL
#define THERM_FASTER_LEVEL 144  // throttle back faster when high
