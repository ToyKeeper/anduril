// Sofirn LT1S Pro
#pragma once

#define MODEL_NUMBER "0623"
#include "hwdef-Sofirn_LT1S-Pro.h"
// ATTINY: 1616

// off mode: low (1)
// lockout: blinking (3)
// Standby power usage:
// - aux high: 6.9 mA (30 days)
// - aux low:  0.16 mA (3.5 years)
// - red moon: 2.17 mA (96 days)
// - white moon: 1.47 mA (141 days)
// Low mode isn't bright enough to be useful on this light,
// but at least it doesn't drain the battery 3X faster than moon mode.
// (it seriously would be more practical to just use moon instead)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 1)

// channel modes...
// CM_WHITE, CM_AUTO, CM_RED, CM_WHITE_RED
#define DEFAULT_CHANNEL_MODE           CM_AUTO

#define FACTORY_RESET_WARN_CHANNEL     CM_RED
#define FACTORY_RESET_SUCCESS_CHANNEL  CM_WHITE

#define CONFIG_WAITING_CHANNEL         CM_RED
#define CONFIG_BLINK_CHANNEL           CM_WHITE

#define POLICE_COLOR_STROBE_CH1        CM_RED
#define POLICE_COLOR_STROBE_CH2        CM_WHITE

// how much to increase total brightness at middle tint
// (0 = 100% brightness, 64 = 200% brightness)
// seems unnecessary on this light
#define TINT_RAMPING_CORRECTION 0

#define RAMP_SIZE 150
// TODO? 200% power at top of ramp on white blend mode
// use dynamic PWM instead of plain 8-bit
// (so we can get lower lows and a smoother ramp)
// (also, red LEDs use a QX7138 chip which has max PWM speed of 10 kHz,
//  and it behaves erratically at full speed,
//  so PWM here is 576 clock cycles long to keep the speed low enough)
// level_calc.py 5.99 1 150 7135 1 0.2 600 --pwm dyn:77:16383:575:3
#define PWM_LEVELS 1,1,2,2,3,4,4,5,6,6,7,8,9,9,10,11,11,12,13,13,14,15,15,16,16,17,18,18,19,19,19,20,20,21,21,21,21,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,23,23,23,23,24,24,24,25,26,26,27,28,29,30,32,33,34,36,38,40,42,44,46,48,50,52,55,57,59,62,65,68,70,73,77,80,83,86,90,94,97,101,105,110,114,118,123,128,133,138,143,148,154,160,166,172,178,185,191,198,205,213,220,228,236,244,252,261,270,279,289,298,308,319,329,340,351,363,374,386,399,411,424,438,452,466,480,495,510,526,542,558,575
#define PWM_TOPS 16383,10869,13246,8043,11458,12772,10093,11043,11450,9664,9991,10091,10048,8868,8838,8730,7814,7724,7589,6864,6748,6604,6024,5899,5398,5287,5159,4754,4638,4287,3963,3876,3594,3511,3265,3038,2829,2770,2586,2417,2260,2115,1981,1857,1742,1636,1537,1445,1360,1281,1207,1138,1073,1013,957,904,855,848,803,760,720,714,677,643,637,630,599,592,585,577,569,579,570,560,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575,575
// shared table for white and red
#define PWM1_LEVELS PWM_LEVELS
#define MAX_1x7135 75
#define MIN_THERM_STEPDOWN 75  // should be above highest dyn_pwm level
#define HALFSPEED_LEVEL 12
#define QUARTERSPEED_LEVEL 5

// the default of 26 looks a bit flat, so increase it
#define CANDLE_AMPLITUDE 40

// override default ramp style
#undef RAMP_STYLE
#define RAMP_STYLE 1  // 0 = smooth, 1 = stepped
// set floor and ceiling as far apart as possible
// because this lantern isn't overpowered
#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  150
//#define RAMP_DISCRETE_FLOOR 17  // 17 50 83 116 150
#define RAMP_DISCRETE_FLOOR 1  // 1 25 50 75 100 125 150
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// LT1S can handle heat well, so don't limit simple mode
//#define SIMPLE_UI_FLOOR 10  // 10 45 80 115 150
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL  RAMP_DISCRETE_CEIL
#define SIMPLE_UI_STEPS RAMP_DISCRETE_STEPS

// Allow 3C (or 6C) in Simple UI (toggle smooth or stepped ramping)
#define USE_SIMPLE_UI_RAMPING_TOGGLE

// allow Aux Config and Strobe Modes in Simple UI
#define USE_EXTENDED_SIMPLE_UI

// turn on at med-low brightness by default (level 50/150, or ramp step 3/7)
// (also sets lockout mode 2H to a useful level)
#define DEFAULT_MANUAL_MEMORY 50
// reset to default after being off for 10 minutes
#define DEFAULT_MANUAL_MEMORY_TIMER 10

// enable 2 click turbo (Anduril 1 style)
#define DEFAULT_2C_STYLE 1

#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

#define USE_POLICE_COLOR_STROBE_MODE
#undef  TACTICAL_LEVELS
#define TACTICAL_LEVELS 120,30,(RAMP_SIZE+3)  // high, low, police strobe

// don't blink while ramping
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif
#ifdef BLINK_AT_RAMP_FLOOR
#undef BLINK_AT_RAMP_FLOOR
#endif
#ifdef BLINK_AT_RAMP_CEIL
#undef BLINK_AT_RAMP_CEIL
#endif
// without this, it's really hard to tell when ramping up stops
#define BLINK_AT_RAMP_CEIL

#define USE_SOFT_FACTORY_RESET
