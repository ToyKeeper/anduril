// the button lights up
#define USE_INDICATOR_LED
// the button is visible while main LEDs are on
#define USE_INDICATOR_LED_WHILE_RAMPING
// enable blinking indicator LED while off
#define TICK_DURING_STANDBY

// not relevant for this driver type
//#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V

// don't blink during ramp, it's irrelevant and annoying on this light
#undef BLINK_AT_CHANNEL_BOUNDARIES
#undef BLINK_AT_RAMP_CEILING
#undef BLINK_AT_RAMP_FLOOR

//#undef USE_SET_LEVEL_GRADUALLY

// use 2.0 A as the ceiling, 2.5 A only for turbo
// start both ramps at the bottom; even moon throws a long way on the GT
#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL POWER_80PX
#define RAMP_DISCRETE_FLOOR 1
#define RAMP_DISCRETE_CEIL POWER_80PX
#define RAMP_DISCRETE_STEPS 7

