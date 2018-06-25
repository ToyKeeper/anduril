// Emisar D4S config options for Anduril

// the button lights up
#define USE_INDICATOR_LED
// the aux LEDs are behind the main LEDs
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif
// enable blinking indicator LED while off
#define TICK_DURING_STANDBY

// ceiling is level 120/150
#define RAMP_SMOOTH_CEIL (MAX_LEVEL*4/5)

// thermal regulation parameters
#undef MIN_THERM_STEPDOWN  // this should be lower, because 3x7135 instead of 1x7135
#define MIN_THERM_STEPDOWN 60  // lowest value it'll step down to
#undef THERM_DOUBLE_SPEED_LEVEL  // this should be lower too, because this light is a hot rod
#define THERM_DOUBLE_SPEED_LEVEL (RAMP_LENGTH*2/3)  // throttle back faster when high

