// Sofirn LT1S Pro

#define MODEL_NUMBER "0623"
#include "hwdef-Sofirn_LT1S-Pro.h"
// ATTINY: 1616

// off mode: high (2)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 2)

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
// seems unnecessary on this light
#define TINT_RAMPING_CORRECTION 0

#define RAMP_SIZE 150
// TODO? 200% power at top of ramp on white blend mode
// use dynamic PWM instead of plain 8-bit
// (so we can get lower lows and a smoother ramp)
// level_calc.py 5.99 1 150 7135 1 0.2 600 --pwm dyn:74:16383:511
// (with a few manual tweaks at the mid-point)
#define PWM_LEVELS 1,1,2,3,3,4,5,6,7,8,9,10,11,13,14,16,17,19,20,22,24,26,28,30,32,34,36,38,41,43,46,48,51,54,56,59,62,65,67,70,73,76,79,82,84,87,90,92,95,97,99,101,103,105,106,107,108,108,109,108,108,107,105,103,101,97,93,89,83,77,70,62,53,43,36,37,38,39,40,42,44,46,48,50,52,54,56,58,60,63,65,68,71,74,77,80,83,87,90,94,98,101,105,109,114,118,123,127,132,137,142,147,153,158,164,170,176,183,189,196,203,210,217,224,232,240,248,257,265,274,283,293,302,312,322,333,343,354,366,377,389,401,414,427,440,453,467,481,496,511
#define PWM_TOPS 16383,11015,13411,15497,11274,12834,13512,13749,13735,13565,13292,12948,12555,13284,12747,13087,12495,12621,12010,12007,11928,11790,11609,11395,11155,10895,10622,10338,10307,9996,9905,9581,9452,9302,8973,8806,8627,8440,8124,7935,7743,7549,7354,7159,6883,6696,6511,6260,6084,5851,5628,5414,5210,5014,4782,4562,4355,4120,3937,3694,3501,3288,3060,2848,2651,2418,2202,2003,1775,1566,1353,1140,926,713,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511
// shared table for white and red
#define PWM1_LEVELS PWM_LEVELS
#define MAX_1x7135 75
// FIXME: clock at 5 MHz w/ full+half+quarter speeds,
// instead of 10 MHz with w/ only half+quarter
// (10 MHz is just wasting power)
// red LEDs use a QX7138 chip which has max PWM speed of 10 kHz,
// so PWM is 512 clock cycles long to avoid running faster than that
#define HALFSPEED_LEVEL 12
#define QUARTERSPEED_LEVEL 5

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 40

// override default ramp style
#undef RAMP_STYLE
#define RAMP_STYLE 1  // 0 = smooth, 1 = stepped
// set floor and ceiling as far apart as possible
// because this lantern isn't overpowered
#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  150
//#define RAMP_DISCRETE_FLOOR 17  // 17 50 83 116 150
#define RAMP_DISCRETE_FLOOR 1  // 1 25 50 75 100 125 150
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// LT1S can handle heat well, so don't limit simple mode
//#define SIMPLE_UI_FLOOR 10  // 10 45 80 115 150
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL  RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS RAMP_DISCRETE_STEPS

// Allow 3C (or 6C) in Simple UI (toggle smooth or stepped ramping)
#define USE_SIMPLE_UI_RAMPING_TOGGLE

// allow Aux Config and Strobe Modes in Simple UI
#define USE_EXTENDED_SIMPLE_UI

// enable 2 click turbo (Anduril 1 style)
#define DEFAULT_2C_STYLE 1

#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

#define USE_POLICE_COLOR_STROBE_MODE
#undef  TACTICAL_LEVELS
#define TACTICAL_LEVELS 120,30,(RAMP_SIZE+3)  // high, low, police strobe

// FIXME: thermal regulation should actually work fine on this light
#ifdef USE_THERMAL_REGULATION
#undef USE_THERMAL_REGULATION
#endif

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif
#ifdef BLINK_AT_RAMP_FLOOR
#undef BLINK_AT_RAMP_FLOOR
#endif
#ifdef BLINK_AT_RAMP_CEIL
#undef BLINK_AT_RAMP_CEIL
#endif
// without this, it's really hard to tell when ramping up stops
#define BLINK_AT_RAMP_CEIL

#define USE_SOFT_FACTORY_RESET
