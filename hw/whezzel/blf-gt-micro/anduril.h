#pragma once
#include "lumintop/blf-gt-mini/anduril.h"
#include "whezzel/anduril.h"

#ifdef USE_BUTTON_LED
    #undef USE_BUTTON_LED
#endif

#ifdef USE_INDICATOR_LED
    #undef USE_INDICATOR_LED
#endif

#ifdef DEFAULT_MANUAL_MEMORY_TIMER
    #undef DEFAULT_MANUAL_MEMORY_TIMER
#endif
#define DEFAULT_MANUAL_MEMORY_TIMER 0

#ifdef DEFAULT_AUTOLOCK_TIME
    #undef DEFAULT_AUTOLOCK_TIME
#endif
#define DEFAULT_AUTOLOCK_TIME 0
