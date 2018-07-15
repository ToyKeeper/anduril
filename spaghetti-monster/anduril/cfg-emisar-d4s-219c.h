// Emisar D4S-219c config options for Anduril
// same as D4S but with FET modes limited to 80% power
// to avoid destroying the LEDs
#include "cfg-emisar-d4s.h"

#undef PWM2_LEVELS
#define PWM2_LEVELS PWM2_LEVELS_219c

