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
// level_calc.py 5.01 1 150 7135 1 0.2 2000 --pwm dyn:75:16383:511
// abstract ramp (power is split between both sets of LEDs)
#define PWM1_LEVELS 1,1,1,2,2,2,3,4,4,5,6,6,7,8,9,10,12,13,14,16,17,19,20,22,24,26,28,30,33,35,37,40,43,45,48,51,54,57,60,63,66,69,72,75,78,82,85,88,91,93,96,99,101,103,106,107,109,110,111,112,112,112,111,110,108,105,102,98,94,88,82,74,66,57,46,35,37,38,40,42,44,46,48,50,53,55,57,60,63,65,68,71,74,77,80,83,87,90,94,98,102,106,110,114,118,123,128,132,137,142,148,153,159,164,170,176,183,189,196,202,209,216,224,231,239,247,255,263,272,281,290,299,309,318,328,339,349,360,371,382,394,406,418,430,443,456,469,483,497,511
#define PWM_TOPS 16383,13675,10740,15439,11908,8117,12779,14760,12239,13449,14017,11902,12260,12349,12259,12045,12929,12466,11970,12281,11704,11791,11178,11133,11013,10837,10619,10370,10425,10113,9792,9719,9603,9248,9093,8917,8725,8521,8309,8091,7870,7648,7425,7204,6985,6855,6638,6425,6218,5952,5760,5573,5339,5116,4952,4704,4513,4292,4084,3889,3673,3470,3252,3048,2833,2608,2401,2187,1989,1767,1563,1339,1134,929,711,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511
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
