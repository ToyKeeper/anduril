// BLF Lantern config options for Anduril
#define MODEL_NUMBER "0621"
#include "hwdef-BLF_LT1.h"
// ATTINY: 85

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING
// off mode: high (2)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 2)

// the lantern has two PWM channels, but they drive different sets of emitters
// (one channel for warm emitters, one channel for cold)
// so enable a special ramping mode which changes tint instead of brightness
#define USE_TINT_RAMPING
// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
//#define TINT_RAMPING_CORRECTION 26  // prototype, 140%
#define TINT_RAMPING_CORRECTION 10  // production model, 115%
//#define TINT_RAMPING_CORRECTION 0  // none

#ifdef RAMP_LENGTH
#undef RAMP_LENGTH
#endif

// 1-130: 0 to 100% power
// level_calc.py 3.0 1 130 7135 1 30 800 --pwm 255
// 131-150: 101% to 200% power
// level_calc.py 8.69 1 150 7135 1 1 1600 --pwm 510
#define RAMP_LENGTH 150
#define PWM1_LEVELS 1,1,2,2,3,3,4,5,5,6,6,7,8,8,9,10,10,11,12,12,13,14,15,16,17,17,18,19,20,21,22,23,24,25,26,27,28,29,31,32,33,34,35,37,38,39,40,42,43,45,46,47,49,50,52,54,55,57,58,60,62,63,65,67,69,70,72,74,76,78,80,82,84,86,88,90,92,95,97,99,101,104,106,108,111,113,115,118,121,123,126,128,131,134,136,139,142,145,148,150,153,156,159,162,166,169,172,175,178,181,185,188,191,195,198,202,205,209,213,216,220,224,227,231,235,239,243,247,251,255,264,274,284,294,305,316,327,339,351,363,376,389,403,417,431,446,461,477,493,510
#define MAX_1x7135 130
#define DEFAULT_LEVEL 70
#define HALFSPEED_LEVEL 14
#define QUARTERSPEED_LEVEL 5

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 40

// override default ramp style
#undef RAMP_STYLE
#define RAMP_STYLE 1  // 0 = smooth, 1 = stepped
// set floor and ceiling as far apart as possible
// because this lantern isn't overpowered
#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  130
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 5

// LT1 can handle heat well, so don't limit simple mode
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL  RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS RAMP_DISCRETE_STEPS

#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

// the sensor (attiny85) is nowhere near the emitters
// so thermal regulation can't work
#ifdef USE_THERMAL_REGULATION
#undef USE_THERMAL_REGULATION
#endif

// don't blink at floor
#ifdef BLINK_AT_RAMP_FLOOR
#undef BLINK_AT_RAMP_FLOOR
#endif
// blink at 100% power
#ifndef BLINK_AT_RAMP_MIDDLE
#define BLINK_AT_RAMP_MIDDLE
#endif
// blink again at the 200% power / ceil / turbo
#ifndef BLINK_AT_RAMP_CEIL
#define BLINK_AT_RAMP_CEIL
#endif
