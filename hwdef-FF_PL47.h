/* Fireflies PL47 driver layout
 */
// same as a D4S, basically
#include "hwdef-Emisar_D4S.h"
#undef FSM_EMISAR_D4S_DRIVER
#undef FSM_EMISAR_D4_DRIVER

// ... except the PL47 has aux LEDs on pin 7
#ifdef AUXLED_PIN
#undef AUXLED_PIN
#endif
#define AUXLED_PIN   PB2    // pin 7

// ... and switch LEDs on pin 3
#ifdef AUXLED2_PIN
#undef AUXLED2_PIN
#endif
#define AUXLED2_PIN   PB4    // pin 3

// average drop across diode on this hardware
#ifdef VOLTAGE_FUDGE_FACTOR
#undef VOLTAGE_FUDGE_FACTOR
#endif
#define VOLTAGE_FUDGE_FACTOR 7  // add 0.35V
