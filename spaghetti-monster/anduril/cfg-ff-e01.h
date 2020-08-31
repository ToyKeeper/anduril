// Fireflies E01 SST-40 thrower config options for Anduril
// most of the good stuff is in the FFUI config; just copy it
#include "../fireflies-ui/cfg-ff-e01.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0441"

#ifndef BLINK_AT_RAMP_CEIL
#define BLINK_AT_RAMP_CEIL
#endif

// 20, 38, 56, 75, [93], 111, 130 (93 is highest regulated)
// (9 / 45 / 116 / 248 / 467 / 742 / 1280 + 2140 lm)
#undef RAMP_DISCRETE_STEPS
#define RAMP_DISCRETE_STEPS 7

// safe limit ~50% power
// 20 56 [93] 130
#define SIMPLE_UI_FLOOR 20
#define SIMPLE_UI_CEIL 130
#define SIMPLE_UI_STEPS 4

