// Noctigon K1-SBT90.2 config options for Anduril
// (is a K1 host with a KR4-like driver and a really high-powered LED)
#define MODEL_NUMBER "0252"
#include "hwdef-Noctigon_K1-SBT90.h"
#include "hank-cfg.h"
// ATTINY: 1634

// this light can safely run a bit hotter than most
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 55

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
#define USE_AUX_RGB_LEDS_WHILE_ON
#define USE_INDICATOR_LED_WHILE_RAMPING


// brightness:
// 0/1023: 0.35 lm
// 1/1023: 2.56 lm
// max regulated: 1740 lm
// FET: ~3700 lm
// maxreg at 130: level_calc.py cube 2 150 7135 0 2.5 1740 FET 1 10 2565
// maxreg at 120: level_calc.py cube 2 150 7135 0 2.5 1740 FET 1 10 3190
#define RAMP_LENGTH 150
#define PWM1_LEVELS 0,0,1,1,2,2,3,3,4,4,5,6,7,8,9,10,11,13,14,15,17,19,20,22,24,26,28,30,33,35,38,40,43,46,49,52,55,59,62,66,70,74,78,82,86,91,96,100,105,111,116,121,127,133,139,145,151,158,165,172,179,186,193,201,209,217,225,234,243,251,261,270,280,289,299,310,320,331,342,353,364,376,388,400,412,425,438,451,464,478,492,506,521,536,551,566,582,597,614,630,647,664,681,699,717,735,754,772,792,811,831,851,871,892,913,935,956,978,1001,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,51,79,109,138,168,198,229,260,292,324,357,390,423,457,492,527,562,598,634,671,708,746,784,822,861,901,941,982,1023
#define DEFAULT_LEVEL 46
#define MAX_1x7135 120
#define HALFSPEED_LEVEL 10
#define QUARTERSPEED_LEVEL 2

#define RAMP_SMOOTH_FLOOR 3  // level 1 is unreliable
#define RAMP_SMOOTH_CEIL  120
// 10, 28, [46], 65, 83, 101, [120]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit ~33% power
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL 110
#define SIMPLE_UI_STEPS 5

// stop panicking at ~40% power or ~1700 lm
#define THERM_FASTER_LEVEL 120
#define MIN_THERM_STEPDOWN DEFAULT_LEVEL
//#define THERM_NEXT_WARNING_THRESHOLD 16  // accumulate less error before adjusting
//#define THERM_RESPONSE_MAGNITUDE 128  // bigger adjustments

// normal party strobe speed; this driver can pulse very quickly
//#define PARTY_STROBE_ONTIME 2

#define THERM_CAL_OFFSET 5

