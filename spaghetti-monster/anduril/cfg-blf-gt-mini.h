// BLF/Lumintop GT Mini config options for Anduril
#include "hwdef-BLF_GT_Mini.h"
// Same as an Emisar D1S, except it has a lighted button
#include "cfg-emisar-d1s.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0322"

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING

