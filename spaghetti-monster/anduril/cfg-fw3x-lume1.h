/* lume1 for FW3x config options for Anduril
 * Constant current Buck-Boost + FET driver
 * For more information: www.loneoceans.com/labs/
 * Datasheets:
 * - 1634: http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8303-8-bit-AVR-Microcontroller-tinyAVR-ATtiny1634_Datasheet.pdf
 * - 85: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf
 */

// TODO: determine if "03" (Lumintop) is the right prefix,
//       or if there should be a brand ID for LoneOceans
#define MODEL_NUMBER "0314"

#include "hwdef-fw3x-lume1.h"
// ATTINY: 1634

// set this light for 50C thermal ceiling
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 50

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// it has no independent LED in the button unlike emisar d4
//#define USE_BUTTON_LED

// the aux LEDs are front-facing, so turn them off while main LEDs are on
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

// ../../bin/level_calc.py cube 1 149 7135 0 0.5 1000, with 0 appended to the end.
// for FET PWM (PWM2), all values are 0, except for last value of 1023
// (with max_pwm set to 1023)
#define RAMP_LENGTH 150
//#define PWM1_LEVELS 0,0,0,0,1,1,1,1,2,2,2,3,3,4,4,5,5,6,7,7,8,9,10,11,12,13,14,15,16,17,19,20,22,23,25,26,28,30,32,34,36,38,40,42,45,47,49,52,55,58,60,63,66,70,73,76,80,83,87,91,94,98,102,107,111,115,120,124,129,134,139,144,150,155,160,166,172,178,184,190,196,203,209,216,223,230,237,244,252,259,267,275,283,291,299,308,316,325,334,343,353,362,372,382,392,402,412,423,433,444,455,466,478,489,501,513,525,538,550,563,576,589,602,616,630,644,658,672,687,701,716,731,747,762,778,794,810,827,844,861,878,895,913,930,948,967,985,1004,1023,0
#define PWM1_LEVELS 1,1,1,1,2,2,2,2,3,3,3,4,4,5,5,6,6,7,8,8,9,10,11,12,13,14,15,16,17,18,20,21,23,24,26,27,29,31,33,35,37,39,41,43,45,48,50,53,56,58,61,64,67,70,74,77,80,84,88,91,95,99,103,108,112,116,121,125,130,135,140,145,150,156,161,167,173,178,184,191,197,203,210,217,223,230,238,245,252,260,268,275,283,292,300,308,317,326,335,344,353,363,372,382,392,402,413,423,434,445,456,467,478,490,502,514,526,538,551,563,576,589,603,616,630,644,658,672,687,702,717,732,747,763,778,794,811,827,844,861,878,895,913,931,949,967,985,1004,1023,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1023
#define DEFAULT_LEVEL 56
#define MAX_1x7135 149
// TODO: test if underclocking works on lume1
//#define HALFSPEED_LEVEL 14
//#define QUARTERSPEED_LEVEL 5
// don't slow down at low levels; this isn't that sort of light
// (it needs to stay at full speed for the 10-bit PWM to work)
#ifdef USE_DYNAMIC_UNDERCLOCKING
#undef USE_DYNAMIC_UNDERCLOCKING
#endif

// the entire ramp is regulated except turbo; don't blink halfway up
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL 149
// turn on BuckBoost from level 1 to 149, but not 150
// Level 150 is when BuckBoost is off and FET is ON 100%
#define LED_ENABLE_PIN_LEVEL_MIN 1
#define LED_ENABLE_PIN_LEVEL_MAX 149
// 10 33 56 79 102 125 [149]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// Muggle mode: Goal from about ~10 lumens to about ~300+ lumens
// In this case, ramp number 99 is about 1A to the driver
#define MUGGLE_FLOOR RAMP_DISCRETE_FLOOR
#define MUGGLE_CEILING 99

// slow down party strobe; this driver can't pulse for too short a time
#define PARTY_STROBE_ONTIME 4

// optional, makes initial turbo step-down faster so first peak isn't as hot
// FET mode can run very very hot, so be extra careful
//#define THERM_HARD_TURBO_DROP

// stop panicking at ~85% regulated power or ~750 lm
#define THERM_FASTER_LEVEL 140

#define THERM_CAL_OFFSET 0  // not needed due to external sensor

// easier access to thermal config mode, similar to Emisar, Noctigon
#define USE_TENCLICK_THERMAL_CONFIG

// can't reset the normal way because power is connected before the button
#define USE_SOFT_FACTORY_RESET
