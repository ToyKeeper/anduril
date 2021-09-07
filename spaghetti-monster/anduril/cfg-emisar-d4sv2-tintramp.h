// Noctigon K9.3 config options for Anduril
#define MODEL_NUMBER "0261"
#include "hwdef-Noctigon_K9.3.h"
#include "hank-cfg.h"
// ATTINY: 1634
// this model requires some special code
#define OVERRIDES_FILE cfg-noctigon-k9.3.c
#define OVERRIDE_SET_LEVEL
#define OVERRIDE_GRADUAL_TICK
inline void set_level_override(uint8_t level);


// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// the aux LEDs are front-facing, so turn them off while main LEDs are on
//#define USE_AUX_RGB_LEDS_WHILE_ON
// it also has an independent LED in the button
#define USE_BUTTON_LED
// TODO: the whole "indicator LED" thing needs to be refactored into
//       "aux LED(s)" and "button LED(s)" since they work a bit differently
// enabling this option breaks the button LED on D4v2.5
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

// has two channels of independent LEDs
#define USE_TINT_RAMPING
// ... but it doesn't make sense to ramp between; only toggle
#define TINT_RAMP_TOGGLE_ONLY

// main LEDs
//   max regulated: ~1750 lm
//   FET: ~8000 lm
// 2nd LEDs
//   max regulated: ~1500 lm
// maxreg at 120: level_calc.py cube 2 150 7135 0 2.5 1740 FET 1 10 3190
#define RAMP_LENGTH 150
// main LEDs
#define PWM1_LEVELS 0,0,1,1,2,2,3,3,4,4,5,6,7,8,9,10,11,13,14,15,17,19,20,22,24,26,28,30,33,35,38,40,43,46,49,52,55,59,62,66,70,74,78,82,86,91,96,100,105,111,116,121,127,133,139,145,151,158,165,172,179,186,193,201,209,217,225,234,243,251,261,270,280,289,299,310,320,331,342,353,364,376,388,400,412,425,438,451,464,478,492,506,521,536,551,566,582,597,614,630,647,664,681,699,717,735,754,772,792,811,831,851,871,892,913,935,956,978,1001,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,51,79,109,138,168,198,229,260,292,324,357,390,423,457,492,527,562,598,634,671,708,746,784,822,861,901,941,982,1023
// 2nd LEDs
#define PWM3_LEVELS 0,0,1,1,2,2,3,3,4,4,5,5,6,7,8,9,10,11,12,13,15,16,17,18,20,21,23,24,26,27,29,31,33,35,37,39,41,43,45,48,50,53,55,58,61,63,66,69,72,75,79,82,85,89,92,96,100,104,108,112,116,120,125,129,134,138,143,148,153,158,163,169,174,180,185,191,197,203,209,215,222,228,235,242,248,255,263,270,277,285,292,300,308,316,324,333,341,350,359,368,377,386,395,405,414,424,434,444,454,465,475,486,497,508,519,531,542,554,566,578,590,603,615,628,641,654,667,680,694,708,722,736,750,765,779,794,809,825,840,856,872,888,904,920,937,954,971,988,1005,1023
#define DEFAULT_LEVEL 46
#define MAX_1x7135 120
#define MAX_Nx7135 MAX_1x7135
#define HALFSPEED_LEVEL 10
#define QUARTERSPEED_LEVEL 2

#define USE_MANUAL_MEMORY_TIMER_FOR_TINT
//#define DEFAULT_MANUAL_MEMORY        DEFAULT_LEVEL
//#define DEFAULT_MANUAL_MEMORY_TIMER  10

#define RAMP_SMOOTH_FLOOR 3  // level 1 is unreliable (?)
#define RAMP_SMOOTH_CEIL  120
// 10, 28, [46], 65, 83, 101, [120]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit highest regulated power (no FET or turbo)
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS 5

// stop panicking at ~25% power or ~1000 lm
#define THERM_FASTER_LEVEL 100
#define MIN_THERM_STEPDOWN DEFAULT_LEVEL
//#define THERM_NEXT_WARNING_THRESHOLD 16  // accumulate less error before adjusting
//#define THERM_RESPONSE_MAGNITUDE 128  // bigger adjustments

// use the brightest setting for strobe
#define STROBE_BRIGHTNESS MAX_LEVEL
// slow down party strobe; this driver can't pulse for 1ms or less
#define PARTY_STROBE_ONTIME 2

#define BLINK_ONCE_TIME 12  // longer blink, since main LEDs are slow

#define THERM_CAL_OFFSET 5

// for consistency with KR4 (not otherwise necessary though)
#define USE_SOFT_FACTORY_RESET


// work around bizarre bug: lockout mode fails when set to solid color blinking
#define USE_K93_LOCKOUT_KLUDGE
