// Fireflies E07-2 config options for Anduril / FFUI
// mostly the same as PL47
#include "cfg-ff-pl47.h"

// ceiling is level 130/150 (50% power)
#undef RAMP_SMOOTH_CEIL
#define RAMP_SMOOTH_CEIL 130

// 20, 56, 93, 130 (83 is highest regulated)
// (requested config is 1%, 5%, 25%, 50%, double-click-turbo)
// (but this doesn't allow us to hit level 83)
#undef RAMP_DISCRETE_FLOOR
#define RAMP_DISCRETE_FLOOR 20
#undef RAMP_DISCRETE_CEIL
#define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#undef RAMP_DISCRETE_STEPS
#define RAMP_DISCRETE_STEPS 4

// regulate down faster when the FET is active, slower otherwise
#undef THERM_FASTER_LEVEL
#define THERM_FASTER_LEVEL 130  // throttle back faster when high

// play it safe, don't try to regulate above the recommended safe level
#ifndef THERM_HARD_TURBO_DROP
#define THERM_HARD_TURBO_DROP
#endif

