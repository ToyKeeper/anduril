#pragma once
#include "hank/emisar-d4sv2/anduril.h"
#include "whezzel/anduril.h"

#ifdef RGB_LED_LOCKOUT_DEFAULT
    #undef RGB_LED_LOCKOUT_DEFAULT
#endif
#define RGB_LED_LOCKOUT_DEFAULT 0x19

#ifdef RGB_LED_OFF_DEFAULT
    #undef RGB_LED_OFF_DEFAULT
#endif
#define RGB_LED_OFF_DEFAULT 0x19
