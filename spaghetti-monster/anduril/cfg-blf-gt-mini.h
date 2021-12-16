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

// too big, remove stuff to make room
#undef USE_SOS_MODE
//#undef USE_RAMP_AFTER_MOON_CONFIG
//#undef USE_RAMP_SPEED_CONFIG
//#undef USE_VOLTAGE_CORRECTION
//#undef USE_2C_STYLE_CONFIG
//#undef USE_TACTICAL_STROBE_MODE

