#pragma once
#include "hank/noctigon-dm11/boost/anduril.h"
#include "whezzel/anduril.h"

#ifdef DEFAULT_MANUAL_MEMORY
    #undef DEFAULT_MANUAL_MEMORY
#endif
#define DEFAULT_MANUAL_MEMORY DEFAULT_LEVEL

#ifdef DEFAULT_MANUAL_MEMORY_TIMER
    #undef DEFAULT_MANUAL_MEMORY_TIMER
#endif
#define DEFAULT_MANUAL_MEMORY_TIMER 0

#ifdef USE_AUTOLOCK
    #undef USE_AUTOLOCK
#endif
#define USE_AUTOLOCK

#ifdef DEFAULT_AUTOLOCK_TIME
    #undef DEFAULT_AUTOLOCK_TIME
#endif
#define DEFAULT_AUTOLOCK_TIME 0
