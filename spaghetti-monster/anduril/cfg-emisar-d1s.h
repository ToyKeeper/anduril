// Emisar D1S config options for Anduril
#include "hwdef-Emisar_D1S.h"
// same as Emisar D4, mostly
#include "cfg-emisar-d4.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0122"

// safe limit ~50% power
#undef SIMPLE_UI_CEIL
#define SIMPLE_UI_CEIL 120

// stop panicking at ~90% power or ~1200 lm (D1S has a good power-to-thermal-mass ratio)
#ifdef THERM_FASTER_LEVEL
#undef THERM_FASTER_LEVEL
#endif
#define THERM_FASTER_LEVEL 144  // throttle back faster when high
