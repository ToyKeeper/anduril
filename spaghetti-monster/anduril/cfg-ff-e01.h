// Fireflies E01 SST-40 thrower config options for Anduril
// most of the good stuff is in the FFUI config; just copy it
#include "../fireflies-ui/cfg-ff-e01.h"

#ifndef BLINK_AT_RAMP_CEIL
#define BLINK_AT_RAMP_CEIL
#endif

// 20, 38, 56, 75, [93], 111, 130 (93 is highest regulated)
// (9 / 45 / 116 / 248 / 467 / 742 / 1280 + 2140 lm)
#undef RAMP_DISCRETE_STEPS
#define RAMP_DISCRETE_STEPS 7

// shortcut for first-time setup
#define USE_TENCLICK_THERMAL_CONFIG
