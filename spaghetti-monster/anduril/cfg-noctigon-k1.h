// Noctigon K1 config options for Anduril
#define MODEL_NUMBER "0251"
// (originally known as Emisar D1S v2)
#include "hwdef-Noctigon_K1.h"
#include "hank-cfg.h"
// ATTINY: 1634

// this light can safely run a bit hotter than most
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 55

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
#define USE_AUX_RGB_LEDS_WHILE_ON
#define USE_INDICATOR_LED_WHILE_RAMPING


// ../../bin/level_calc.py cube 1 150 7135 1 4 1300
// (with max_pwm set to 1023)
// (level 0 flickers and isn't relevant on a thrower, so it's omitted)
#define RAMP_LENGTH 150
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,6,6,7,8,9,10,11,12,13,14,15,16,17,18,20,21,23,24,26,27,29,31,32,34,36,38,40,43,45,47,49,52,54,57,60,62,65,68,71,74,77,81,84,87,91,95,98,102,106,110,114,118,122,127,131,136,141,145,150,155,160,166,171,176,182,188,193,199,205,211,218,224,231,237,244,251,258,265,272,280,287,295,303,310,319,327,335,344,352,361,370,379,388,397,407,416,426,436,446,457,467,477,488,499,510,521,533,544,556,568,580,592,604,617,629,642,655,668,682,695,709,723,737,751,766,781,795,810,826,841,857,872,888,904,921,937,954,971,988,1005,1023
#define MAX_1x7135 50

// the entire ramp is regulated; don't blink halfway up
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// don't slow down at low levels; this isn't that sort of light
// (it needs to stay at full speed for the 10-bit PWM to work)
#ifdef USE_DYNAMIC_UNDERCLOCKING
#undef USE_DYNAMIC_UNDERCLOCKING
#endif

#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  130
// 10, 30, [50], 70, 90, 110, 130
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit ~75% power
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS 5

// make candle mode wobble more
#define CANDLE_AMPLITUDE 32

// stop panicking at ~70% power or ~600 lm
#define THERM_FASTER_LEVEL 130

#define THERM_RESPONSE_MAGNITUDE 32  // smaller adjustments, this host changes temperature slowly
#define THERM_NEXT_WARNING_THRESHOLD 32  // more error tolerance before adjusting

// slow down party strobe; this driver can't pulse for 1ms or less
#define PARTY_STROBE_ONTIME 2

#define THERM_CAL_OFFSET 5

