// Mateminco/Astrolux MF01S options for Anduril
#include "hwdef-Mateminco_MF01S.h"

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
//#define USE_INDICATOR_LED_WHILE_RAMPING
// enable blinking indicator LED while off
#define TICK_DURING_STANDBY
#define STANDBY_TICK_SPEED 3  // every 0.128 s
#define USE_FANCIER_BLINKING_INDICATOR
// off mode: low (1)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)


// don't blink during ramp, it's irrelevant and annoying on this light
#define BLINK_AT_RAMP_CEILING
#undef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_FLOOR

// measured brightness with 4x30Q cells at 4.11V:
// moon: 2.5 lm
// channel 1: 617 lm
// channel 2: 13500 lm
// ../../../bin/level_calc.py seventh 2 150 7135 1 12 717 FET 3 10 13000
#define RAMP_LENGTH 150
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,5,6,7,7,8,9,10,11,12,13,14,15,16,18,19,20,22,24,25,27,29,31,33,35,38,40,43,46,48,51,55,58,61,65,69,73,77,81,86,90,95,101,106,112,118,124,130,137,144,151,159,167,175,184,193,202,212,222,232,243,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,8,9,10,11,12,13,15,16,17,18,20,21,23,24,26,27,29,31,32,34,36,38,40,42,44,46,49,51,54,56,59,61,64,67,70,73,76,79,82,86,89,93,97,100,104,108,112,117,121,126,130,135,140,145,150,156,161,167,172,178,184,191,197,204,210,217,225,232,239,247,255
#define MAX_1x7135 70  // ~626 lm
#define HALFSPEED_LEVEL 23
#define QUARTERSPEED_LEVEL 6

#define RAMP_SMOOTH_FLOOR 1   // ~2.5 lm
#define RAMP_SMOOTH_CEIL 120  // ~5400 lm
// 20, 36, 53, [70], 86, 103, 120
#define RAMP_DISCRETE_FLOOR 20 // 35 lm
#define RAMP_DISCRETE_CEIL 120 // ~5400 lm
#define RAMP_DISCRETE_STEPS 7  // 35, 108, 280, 626, 1500, 2930, 5400 lm

#define USE_TENCLICK_THERMAL_CONFIG  // by request
#define THERM_FASTER_LEVEL 125  // throttle back faster when high (>6000 lm)
#define THERM_HARD_TURBO_DROP  // this light is massively overpowered

