// Fireflies PL47 config options for Anduril

// the button lights up
#define USE_INDICATOR_LED
// the aux LEDs are in the same place as the main LEDs
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif
//#define USE_INDICATOR_LED_WHILE_RAMPING
// enable blinking indicator LED while off?
#define TICK_DURING_STANDBY

// If TICK_DURING_STANDBY is enabled...
// off mode: high (2)
// lockout: blinking (3)
#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 2)

// the "low" mode doesn't work on this light's aux LEDs
// (but it does work on the switch LEDs)
//#define INDICATOR_LED_SKIP_LOW

// ... or if TICK_DURING_STANDBY is turned off:
// off mode: high (2)
// lockout: off (0)
//#define INDICATOR_LED_DEFAULT_MODE ((0<<2) + 2)


#ifdef RAMP_LENGTH
#undef RAMP_LENGTH
#endif
#define RAMP_LENGTH 150

// driver is a FET + 3x7135, ~400 lm at highest regulated level
// ramp copied from Emisar D4S ramp
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,5,6,6,7,8,8,9,10,10,11,12,13,14,15,16,17,18,19,21,22,23,25,26,27,29,31,32,34,36,38,40,42,44,46,49,51,54,56,59,62,65,68,71,74,78,81,85,89,93,97,101,106,110,115,120,125,130,136,141,147,153,160,166,173,180,187,195,202,210,219,227,236,245,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,4,5,7,8,10,11,13,14,16,18,19,21,23,25,27,29,31,34,36,38,41,43,46,48,51,54,57,60,63,66,69,72,76,79,83,87,91,95,99,103,107,112,116,121,126,131,136,141,146,152,158,163,169,175,182,188,195,202,209,216,223,231,239,247,255
#define MAX_1x7135 83
#define HALFSPEED_LEVEL 13
#define QUARTERSPEED_LEVEL 6

// ceiling is level 120/150
#define RAMP_SMOOTH_CEIL (MAX_LEVEL*4/5)

// thermal regulation parameters
#ifdef MIN_THERM_STEPDOWN
#undef MIN_THERM_STEPDOWN  // this should be lower, because 3x7135 instead of 1x7135
#endif
#define MIN_THERM_STEPDOWN 60  // lowest value it'll step down to

// regulate down faster when the FET is active, slower otherwise
#define THERM_FASTER_LEVEL 130  // throttle back faster when high

// play it safe, don't try to regulate above the recommended safe level
#define THERM_HARD_TURBO_DROP

// don't do this
#undef BLINK_AT_CHANNEL_BOUNDARIES
#undef BLINK_AT_RAMP_CEILING

