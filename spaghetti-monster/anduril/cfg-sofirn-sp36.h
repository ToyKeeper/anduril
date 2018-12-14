// Sofirn SP36 (small Q8) config options for Anduril

// voltage readings were a little high with the Q8 value
#define VOLTAGE_FUDGE_FACTOR 5  // add 0.25V, not 0.35V

#define FSM_BLF_Q8_DRIVER
#include "cfg-blf-q8.h"

// the high button LED mode on this light uses too much power
// off mode: low (1)
// lockout: blinking (3)
#ifdef INDICATOR_LED_DEFAULT_MODE
#undef INDICATOR_LED_DEFAULT_MODE
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)
#endif

// don't blink during the ramp; the button LED brightness is sufficient
// to indicate which power channel(s) are being used
#ifdef BLINK_AT_CHANNEL_BOUNDARIES
#undef BLINK_AT_CHANNEL_BOUNDARIES
#endif
#ifdef BLINK_AT_RAMP_CEILING
#undef BLINK_AT_RAMP_CEILING
#endif

// stop panicking at ~60% power or ~3000 lm
#ifdef THERM_FASTER_LEVEL
#undef THERM_FASTER_LEVEL
#endif
#define THERM_FASTER_LEVEL 130

// be extra-careful at high levels
// (or not... this host seems to heat up pretty slowly)
//#ifndef THERM_HARD_TURBO_DROP
//#define THERM_HARD_TURBO_DROP
//#endif

