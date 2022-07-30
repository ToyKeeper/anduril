// Emisar D1v2 (linear+FET) config options for Anduril
// (2022 re-issue / update of old D1)
// ATTINY: 1634
// similar to a Noctigon KR4, sort of
#include "cfg-noctigon-kr4.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0124"

// ... there is no separate button LED, only the RGB LEDs
#ifdef USE_BUTTON_LED
#undef USE_BUTTON_LED
#endif
// the aux LEDs are in the button, so use them while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING

// safe limit: max regulated power
#undef SIMPLE_UI_CEIL
#define SIMPLE_UI_CEIL MAX_1x7135

// stop panicking at ~75% power or ~1000 lm (D1 has a decent power-to-thermal-mass ratio)
#ifdef THERM_FASTER_LEVEL
#undef THERM_FASTER_LEVEL
#endif
#define THERM_FASTER_LEVEL (RAMP_SIZE*9/10)  // throttle back faster when high
