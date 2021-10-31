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
// abstract ramp (power is split between both sets of LEDs)
// 1-130: 0 to 100% power
// level_calc.py 5.01 1 130 7135 2 0.2 2000 --pwm dyn:64:16383:511
// 131 to 150: 101% to 200% power
// level_calc.py 6.44 1 150 7135 1 0.2 2000 --pwm dyn:74:16383:1022
#define PWM1_LEVELS 2,2,2,3,3,4,4,5,6,7,8,9,10,11,13,14,16,17,19,21,23,25,28,30,33,35,38,41,44,47,50,54,57,60,64,67,71,74,78,81,84,88,91,94,97,99,101,103,105,106,107,107,107,106,105,102,99,95,90,84,77,68,58,47,34,36,38,40,42,44,47,49,52,54,57,60,63,66,69,73,76,80,83,87,91,96,100,104,109,114,119,124,130,135,141,147,153,160,166,173,180,187,195,203,211,219,228,236,245,255,264,274,285,295,306,317,329,340,353,365,378,391,405,419,433,448,463,479,495,511,530,550,570,591,612,634,657,680,705,730,755,782,809,837,865,895,925,957,989,1022
#define PWM_TOPS 16383,13234,9781,13826,9593,13434,9973,12021,12900,13193,13150,12899,12508,12023,12666,11982,12181,11422,11393,11247,11018,10731,10826,10434,10365,9927,9767,9565,9332,9076,8806,8693,8395,8096,7928,7626,7439,7143,6948,6665,6393,6203,5946,5700,5465,5187,4926,4681,4451,4195,3957,3700,3463,3213,2983,2718,2476,2231,1986,1742,1501,1245,997,756,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511
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
#define THERM_FASTER_LEVEL 130
#define MIN_THERM_STEPDOWN 65  // should be above highest dyn_pwm level

// use the brightest setting for strobe
#define STROBE_BRIGHTNESS MAX_LEVEL
// slow down party strobe; this driver can't pulse for 1ms or less
#define PARTY_STROBE_ONTIME 2

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 33

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
