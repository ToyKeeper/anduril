// Emisar D1v2 (linear+FET) config options for Anduril
// (2022 re-issue / update of old D1)
// ATTINY: 1634
// similar to a Noctigon KR4, sort of
#include "cfg-noctigon-kr4.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0124"

// some models use a simple button LED, others use RGB...
// ... so include support for both
#define USE_BUTTON_LED
// the aux LEDs are in the button, so use them while main LEDs are on
#define USE_AUX_RGB_LEDS
#define USE_AUX_RGB_LEDS_WHILE_ON
#define USE_INDICATOR_LED_WHILE_RAMPING

// safe limit: max regulated power
#undef SIMPLE_UI_CEIL
#define SIMPLE_UI_CEIL MAX_1x7135

// stop panicking at ~75% power or ~1000 lm (D1 has a decent power-to-thermal-mass ratio)
#ifdef THERM_FASTER_LEVEL
#undef THERM_FASTER_LEVEL
#endif
#define THERM_FASTER_LEVEL (RAMP_SIZE*9/10)  // throttle back faster when high


// work around bizarre bug: lockout mode fails when set to solid color blinking
#define USE_K93_LOCKOUT_KLUDGE
