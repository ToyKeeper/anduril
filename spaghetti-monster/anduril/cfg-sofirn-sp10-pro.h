// Sofirn SP10 Pro config options for Anduril
#define MODEL_NUMBER "0631"
#include "hwdef-Sofirn_SP10-Pro.h"
// ATTINY: 1616

#undef BLINK_AT_RAMP_MIDDLE

#define USE_DYNAMIC_UNDERCLOCKING

// PWM is mostly a "ninth" ramp from level_calc.py but with some extra lower values at the beginning of the ramp
#define RAMP_LENGTH 150
#define PWM1_LEVELS 255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define PWM2_LEVELS 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,5,5,5,5,6,6,6,7,7,7,8,8,8,9,9,10,10,10,11,11,12,12,13,14,14,15,16,16,17,18,18,19,20,21,22,23,24,25,26,27,28,29,30,31,33,34,35,37,38,40,41,43,45,46,48,50,52,54,56,58,60,63,65,67,70,73,75,78,81,84,87,90,93,97,100,104,107,111,115,119,123,128,132,137,141,146,151,156,162,167,173,179,185,191,197,204,210,217,224,232,239,247,255

#define MAX_1x7135 13
#define HALFSPEED_LEVEL 14
#define QUARTERSPEED_LEVEL 6

#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL 120
#define RAMP_DISCRETE_FLOOR RAMP_SMOOTH_FLOOR
#define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// at Sofirn's request, use max (150) for the Simple UI ceiling
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL 150
#define SIMPLE_UI_STEPS 5

// also at Sofirn's request, enable 2 click turbo
#define USE_2C_MAX_TURBO

// enable SOS in the blinkies group
#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE

// and finally, set the default ramp style to Stepped
#undef RAMP_STYLE
#define RAMP_STYLE 1  // 0 = smooth, 1 = stepped

// stop panicking at ~30% power
#define THERM_FASTER_LEVEL 105