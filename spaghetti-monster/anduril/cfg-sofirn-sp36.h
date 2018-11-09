// Sofirn SP36 (small Q8) config options for Anduril

#define FSM_BLF_Q8_DRIVER
#include "cfg-blf-q8.h"

// stop panicking at ~50% power or ~2000 lm
#ifdef THERM_FASTER_LEVEL
#undef THERM_FASTER_LEVEL
#endif
#define THERM_FASTER_LEVEL 125

// be extra-careful at high levels
#ifndef THERM_HARD_TURBO_DROP
#define THERM_HARD_TURBO_DROP
#endif

