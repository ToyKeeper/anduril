// Fireflies E01 SST-40 thrower config options for Anduril
#include "cfg-ff-pl47.h"

// disable indicator LED
#undef USE_INDICATOR_LED
#undef TICK_DURING_STANDBY

// ceiling is level 130/150  (50% power)
#undef RAMP_SMOOTH_CEIL
#define RAMP_SMOOTH_CEIL 130
#define BLINK_AT_RAMP_CEILING

// 13, 36, 59, [83], 106, 130 (83 is highest regulated)
#undef RAMP_DISCRETE_FLOOR
#define RAMP_DISCRETE_FLOOR 13
#undef RAMP_DISCRETE_CEIL
#define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#undef RAMP_DISCRETE_STEPS
#define RAMP_DISCRETE_STEPS 6

// regulate down faster when the FET is active, slower otherwise
#undef THERM_FASTER_LEVEL
#define THERM_FASTER_LEVEL 130  // throttle back faster when high

// play it safe, don't try to regulate above the recommended safe level
#ifndef THERM_HARD_TURBO_DROP
#define THERM_HARD_TURBO_DROP
#endif

// shortcut for first-time setup
#define USE_TENCLICK_THERMAL_CONFIG
