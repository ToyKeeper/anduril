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
// level_calc.py 5.01 1 150 7135 1 0.2 2000 --pwm dyn:80:16383:255
// abstract ramp (power is split between both sets of LEDs)
#define PWM1_LEVELS 1,1,1,2,2,2,3,4,4,5,6,6,7,8,9,10,12,13,14,16,17,19,21,23,25,27,29,31,33,36,38,41,44,47,50,53,56,59,62,66,69,73,76,80,83,87,90,94,98,101,104,108,111,114,117,120,122,125,127,129,130,131,132,132,132,131,130,128,125,122,118,113,108,101,93,84,75,63,51,37,22,23,24,26,27,28,29,30,32,33,34,36,37,39,40,42,44,46,47,49,51,53,55,57,60,62,64,66,69,71,74,77,80,82,85,88,91,95,98,101,105,108,112,116,120,123,128,132,136,140,145,149,154,159,164,169,174,180,185,191,197,203,209,215,221,228,234,241,248,255
#define PWM_TOPS 16383,13682,10747,15453,11909,8095,12781,14776,12231,13453,14029,11886,12249,12339,12249,12032,12939,12468,11964,12288,11697,11793,11770,11661,11489,11268,11012,10729,10425,10411,10063,9968,9833,9668,9479,9273,9054,8825,8590,8482,8233,8097,7843,7691,7439,7278,7033,6869,6702,6470,6245,6085,5872,5668,5470,5280,5056,4882,4678,4485,4268,4064,3872,3664,3468,3260,3066,2863,2652,2457,2257,2054,1866,1659,1453,1248,1060,847,651,448,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
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
#define MIN_THERM_STEPDOWN 80

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
