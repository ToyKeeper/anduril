// Emisar D1 config options for Anduril
#include "hwdef-Emisar_D1.h"
// same as Emisar D4, mostly
#include "cfg-emisar-d4.h"

// stop panicking at ~75% power or ~1000 lm (D1 has a decent power-to-thermal-mass ratio)
#ifdef THERM_FASTER_LEVEL
#undef THERM_FASTER_LEVEL
#endif
#define THERM_FASTER_LEVEL (RAMP_SIZE*9/10)  // throttle back faster when high
