// spaghetti-monster.c: UI toolkit / microkernel for e-switch flashlights.
// Copyright (C) 2017-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

////////// include all the .c files //////////

#include "arch/mcu.c"

#ifdef USE_DEBUG_BLINK
void debug_blink(uint8_t num) {
    for(; num>0; num--) {
        PWM1_LVL = 32;
        delay_4ms(100/4);
        PWM1_LVL = 0;
        delay_4ms(100/4);
    }
}
#endif

#include "fsm/states.c"
#include "fsm/events.c"
#include "fsm/adc.c"
#include "fsm/wdt.c"
#include "fsm/pcint.c"
#include "fsm/standby.c"
#include "fsm/channels.c"
#include "fsm/ramping.c"
#include "fsm/random.c"
#ifdef USE_EEPROM
#include "fsm/eeprom.c"
#endif
#include "fsm/misc.c"
#include "fsm/main.c"

