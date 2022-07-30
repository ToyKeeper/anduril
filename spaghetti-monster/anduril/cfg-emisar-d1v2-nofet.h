// Emisar D1v2 (linear+FET) config options for Anduril
// (2022 re-issue / update of old D1)
// ATTINY: 1634
// similar to a Noctigon KR4, sort of
#include "cfg-noctigon-kr4-nofet.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0125"

// ... there is no separate button LED, only the RGB LEDs
#ifdef USE_BUTTON_LED
#undef USE_BUTTON_LED
#endif
// the aux LEDs are in the button, so use them while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING

// safe limit: same as regular ramp
#undef SIMPLE_UI_CEIL
#define SIMPLE_UI_CEIL RAMP_SMOOTH_CEIL
