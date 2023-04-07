// Sofirn SC21 Pro - same setup as a Wurkkos TS10, but with the aux indicator on while ramping
#include "cfg-wurkkos-ts10.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0632"
// ATTINY: 1616

// turn on the aux LED while main LED is on
#ifndef USE_INDICATOR_LED_WHILE_RAMPING
#define USE_INDICATOR_LED_WHILE_RAMPING
#endif
