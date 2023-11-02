// fsm-standby.h: standby mode functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// deferred "off" so we won't suspend in a weird state
// (like...  during the middle of a strobe pulse)
// set this to nonzero to enter standby mode next time the system is idle
volatile uint8_t go_to_standby = 0;

#ifdef TICK_DURING_STANDBY
#ifndef STANDBY_TICK_SPEED
#define STANDBY_TICK_SPEED 3  // every 0.128 s
/*
 * From the Attiny85 manual:
 * 0: 16 ms
 * 1: 32 ms
 * 2: 64 ms
 * 3: 0.128 s
 * 4: 0.256 s
 * 5: 0.512 s
 * 6: 1.0 s
 * 7: 2.0 s
 * 32: 4.0 s
 * 33: 8.0 s
 * (other values may have unexpected effects; not sure why the final bit is
 *  separated from the others, in the "32" position instead of "8", but that's
 *  how it is)
 */
#endif

#if (STANDBY_TICK_SPEED == 1)
#define SLEEP_TICKS_PER_SECOND 31
#define SLEEP_TICKS_PER_MINUTE 1800

#elif (STANDBY_TICK_SPEED == 2)
#define SLEEP_TICKS_PER_SECOND 16
#define SLEEP_TICKS_PER_MINUTE 900

#elif (STANDBY_TICK_SPEED == 3)
#define SLEEP_TICKS_PER_SECOND 8
#define SLEEP_TICKS_PER_MINUTE 450

#elif (STANDBY_TICK_SPEED == 4)
#define SLEEP_TICKS_PER_SECOND 4
#define SLEEP_TICKS_PER_MINUTE 225

#elif (STANDBY_TICK_SPEED == 5)
#define SLEEP_TICKS_PER_SECOND 2
#define SLEEP_TICKS_PER_MINUTE 113

#elif (STANDBY_TICK_SPEED == 6)
#define SLEEP_TICKS_PER_SECOND 1
#define SLEEP_TICKS_PER_MINUTE 57

#endif
#endif

#define standby_mode sleep_until_eswitch_pressed
void sleep_until_eswitch_pressed();

#ifdef USE_IDLE_MODE
// stops processing until next click or timer tick
// (I think)
void idle_mode();
#endif

