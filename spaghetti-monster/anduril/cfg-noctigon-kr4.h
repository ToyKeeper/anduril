// Noctigon KR4 config options for Anduril
#include "hwdef-Noctigon_KR4.h"
// ATTINY: 1634

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
//#define USE_AUX_RGB_LEDS_WHILE_ON
//#define USE_INDICATOR_LED_WHILE_RAMPING
#define RGB_LED_OFF_DEFAULT 0x18  // low, voltage
#define RGB_LED_LOCKOUT_DEFAULT 0x37  // blinking, rainbow

// enable blinking aux LEDs
#define TICK_DURING_STANDBY
#define STANDBY_TICK_SPEED 3  // every 0.128 s


// brightness w/ SST-20 4000K LEDs:
// 0/1023: 0.35 lm
// 1/1023: 2.56 lm
// max regulated: 1740 lm
// FET: ~3700 lm
// level_calc.py 3.0 2 150 7135 0 2.5 1740 FET 1 10 2565
// (with max_pwm set to 1023)
// (designed to hit max regulated at 130/150)
#define RAMP_LENGTH 150
#define PWM1_LEVELS 0,0,1,1,2,2,3,3,4,4,5,5,6,7,8,9,10,11,12,13,15,16,17,19,21,22,24,26,28,30,32,34,36,39,41,44,47,49,52,55,58,62,65,68,72,76,80,84,88,92,96,101,105,110,115,120,125,131,136,142,147,153,159,166,172,179,185,192,199,206,214,221,229,237,245,253,262,270,279,288,298,307,317,326,336,346,357,367,378,389,401,412,424,436,448,460,472,485,498,511,525,539,552,567,581,596,611,626,641,657,673,689,705,722,739,756,773,791,809,827,846,864,883,903,922,942,962,983,1003,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,130,177,225,273,323,372,423,474,526,578,632,686,740,795,851,908,965,1023
#define MAX_1x7135 50
#define MAX_Nx7135 130

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
#define RAMP_SMOOTH_CEIL 130
// 10, 30, [50], 70, 90, 110, 130
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

#define MUGGLE_FLOOR RAMP_DISCRETE_FLOOR
#define MUGGLE_CEILING 70

// optional, makes initial turbo step-down faster so first peak isn't as hot
// the KR4 runs very very hot on turbo, so be extra careful
//#define THERM_HARD_TURBO_DROP

// stop panicking at ~70% power or ~600 lm
#define THERM_FASTER_LEVEL 130
// respond to thermal changes faster
#define THERMAL_WARNING_SECONDS 3
#define THERMAL_UPDATE_SPEED 1
#define THERM_PREDICTION_STRENGTH 4

// easier access to thermal config mode, for Noctigon
#define USE_TENCLICK_THERMAL_CONFIG

// slow down party strobe; this driver can't pulse for 1ms or less
#define PARTY_STROBE_ONTIME 2

#define THERM_CAL_OFFSET 5

// attiny1634 has enough space to smooth out voltage readings
// (prevent the button from blinking during use)
//#define USE_VOLTAGE_LOWPASS

