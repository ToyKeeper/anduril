// Fireflies ROT66 config options for Anduril

// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING
// enable blinking indicator LED while off?
//#define TICK_DURING_STANDBY

// If TICK_DURING_STANDBY is enabled...
// off mode: high (2)
// lockout: blinking (3)
//#define INDICATOR_LED_DEFAULT_MODE ((3<<2) + 2)

// ... or if TICK_DURING_STANDBY is turned off:
// off mode: high (2)
// lockout: off (0)
#define INDICATOR_LED_DEFAULT_MODE ((0<<2) + 2)


#ifdef RAMP_LENGTH
#undef RAMP_LENGTH
#endif

// driver is a FET+N+1,
// where N=6 for the 219b version,
// or N=13 for the XP-L HI version
// calculated to get transition points at level 65 and 115, using a "ninth"-shaped ramp
#define RAMP_LENGTH 150
// first 65 steps copied from FW3A
#define PWM1_LEVELS 1,1,2,2,3,3,4,4,5,6,7,8,9,10,12,13,14,15,17,19,20,22,24,26,29,31,34,36,39,42,45,48,51,55,59,62,66,70,75,79,84,89,93,99,104,110,115,121,127,134,140,147,154,161,168,176,184,192,200,209,217,226,236,245,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
// remaining steps from this command:
// level_calc.py 3 150 7135 1 1.4 110.1 7135 14 1 1058.5 FET 1 10 4500
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,17,18,20,21,23,25,27,29,31,33,36,38,41,43,46,49,52,55,58,62,65,69,73,77,81,86,90,95,100,105,111,116,122,128,135,141,148,155,163,170,178,187,195,204,214,224,234,244,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0
#define PWM3_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,8,12,16,21,25,30,35,40,45,50,56,62,68,74,81,87,94,101,109,116,124,133,141,150,159,168,178,188,198,209,220,231,243,255
#define MAX_1x7135 65
#define MAX_Nx7135 115
#define HALFSPEED_LEVEL 14
#define QUARTERSPEED_LEVEL 8

// regulate down faster when the FET is active, slower otherwise
#define THERM_FASTER_LEVEL 130  // throttle back faster when high

// play it safe, don't try to regulate above the recommended safe level
#define THERM_HARD_TURBO_DROP

// don't do this
#undef BLINK_AT_CHANNEL_BOUNDARIES
#undef BLINK_AT_RAMP_CEILING

