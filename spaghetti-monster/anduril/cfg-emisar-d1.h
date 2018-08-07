// Emisar D1 config options for Anduril
#include "cfg-emisar-d4.h"

// stop panicking at ~75% power or ~1000 lm (D1 has a decent power-to-thermal-mass ratio)
#undef THERM_FASTER_LEVEL
#define THERM_FASTER_LEVEL (RAMP_SIZE*9/10)  // throttle back faster when high
