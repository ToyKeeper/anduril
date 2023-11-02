// fsm-wdt.h: WDT (Watch Dog Timer) functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define TICKS_PER_SECOND 62

void WDT_on();
inline void WDT_off();

volatile uint8_t irq_wdt = 0;  // WDT interrupt happened?

#ifdef TICK_DURING_STANDBY
  #if defined(USE_INDICATOR_LED) || defined(USE_AUX_RGB_LEDS)
  // measure battery charge while asleep
  #define USE_SLEEP_LVP
  #endif
#endif

