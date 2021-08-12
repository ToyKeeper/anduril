// Sofirn SP10 Pro config options for Anduril
#define MODEL_NUMBER "0631"
#include "hwdef-Sofirn_SP10-Pro.h"
// ATTINY: 1616

#undef BLINK_AT_RAMP_MIDDLE

#define USE_DYNAMIC_UNDERCLOCKING

#define RAMP_LENGTH 150
#define PWM1_LEVELS 255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define PWM2_LEVELS 0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,5,5,5,6,6,6,7,7,8,8,8,9,9,10,10,11,11,12,13,13,14,14,15,16,16,17,18,19,19,20,21,22,23,23,24,25,26,27,28,29,30,31,32,33,34,35,37,38,39,40,41,43,44,45,47,48,49,51,52,54,55,57,58,60,61,63,65,66,68,70,72,74,75,77,79,81,83,85,87,89,91,93,96,98,100,102,105,107,109,112,114,117,119,122,124,127,129,132,135,138,140,143,146,149,152,155,158,161,164,167,170,174,177,180,184,187,190,194,197,201,204,208,212,215,219,223,227,231,235,239,243,247,251,255

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