// Emisar D4S V2 tint-ramping config options for Anduril (based on Noctigon K9.3)
#define MODEL_NUMBER "0135"
#include "hwdef-Emisar_D4Sv2-tintramp.h"
#include "hank-cfg.h"
// ATTINY: 1634

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// the aux LEDs are front-facing, so turn them off while main LEDs are on
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
// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
#define TINT_RAMPING_CORRECTION 0  // none, linear regulator doesn't need it

// main LEDs
//   output: unknown, 2000 lm?
//   FET: absent / unused?
// 2nd LEDs
//   output: unknown, 2000 lm?
#define RAMP_LENGTH 150
// level_calc.py 5.01 1 150 7135 1 0.2 2000 --pwm dyn:74:16383:511
// abstract ramp (power is split between both sets of LEDs)
#define PWM1_LEVELS 1,1,1,2,2,2,3,4,4,5,6,6,7,8,9,10,12,13,14,16,17,19,20,22,24,26,28,30,32,35,37,40,42,45,47,50,53,56,59,62,65,68,71,74,77,80,83,86,89,91,94,96,98,100,102,104,105,106,107,107,107,106,105,103,101,98,94,90,84,78,71,63,54,44,33,35,37,38,40,42,44,46,48,50,53,55,57,60,63,65,68,71,74,77,80,83,87,90,94,98,102,106,110,114,118,123,128,132,137,142,148,153,159,164,170,176,183,189,196,202,209,216,224,231,239,247,255,263,272,281,290,299,309,318,328,339,349,360,371,382,394,406,418,430,443,456,469,483,497,511
#define PWM_TOPS 16383,13673,10738,15435,11908,8123,12779,14756,12240,13447,14013,11907,12263,12351,12261,12048,12926,12464,11972,12278,11704,11789,11180,11134,11013,10837,10620,10371,10100,10113,9793,9718,9376,9248,8898,8738,8560,8369,8168,7961,7749,7535,7321,7107,6895,6686,6480,6278,6080,5823,5639,5403,5178,4965,4763,4570,4346,4134,3936,3714,3507,3283,3074,2853,2648,2433,2211,2006,1776,1564,1351,1137,924,714,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511
#define DEFAULT_LEVEL 70
#define MAX_1x7135 150
#define HALFSPEED_LEVEL 10
#define QUARTERSPEED_LEVEL 2

#define USE_MANUAL_MEMORY_TIMER_FOR_TINT
//#define DEFAULT_MANUAL_MEMORY        DEFAULT_LEVEL
//#define DEFAULT_MANUAL_MEMORY_TIMER  10

#define RAMP_SMOOTH_FLOOR 10  // level 1 is unreliable (?)
#define RAMP_SMOOTH_CEIL  130
// 10, 30, 50, [70], 90, 110, [130]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit highest regulated power (no FET or turbo)
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS 5

// stop panicking at ~1500 lm
#define THERM_FASTER_LEVEL 140
#define MIN_THERM_STEPDOWN 75  // should be above highest dyn_pwm level

// use the brightest setting for strobe
#define STROBE_BRIGHTNESS MAX_LEVEL
// slow down party strobe; this driver can't pulse for 1ms or less
#define PARTY_STROBE_ONTIME 2

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 40

// the power regulator is a bit slow, so push it harder for a quick response from off
#define DEFAULT_JUMP_START_LEVEL 21
#define BLINK_BRIGHTNESS DEFAULT_LEVEL
#define BLINK_ONCE_TIME 12  // longer blink, since main LEDs are slow

#define THERM_CAL_OFFSET 5

#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// for consistency with KR4 (not otherwise necessary though)
#define USE_SOFT_FACTORY_RESET


// work around bizarre bug: lockout mode fails when set to solid color blinking
#define USE_K93_LOCKOUT_KLUDGE
