// fsm-events.h: Event-handling functions for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <avr/pgmspace.h>


// timeout durations in ticks (each tick 1/62th s)
#ifndef HOLD_TIMEOUT
#define HOLD_TIMEOUT 24
#endif
#ifndef RELEASE_TIMEOUT
#define RELEASE_TIMEOUT 18
#endif

// return codes for Event handlers
// Indicates whether this handler consumed (handled) the Event, or
// if the Event should be sent to the next handler in the stack.
#define EVENT_HANDLED 0
#define EVENT_NOT_HANDLED 1
#define MISCHIEF_MANAGED EVENT_HANDLED
#define MISCHIEF_NOT_MANAGED EVENT_NOT_HANDLED

// typedefs
typedef uint8_t Event;
typedef struct Emission {
    Event event;
    uint16_t arg;
} Emission;

Event current_event;
// at 0.016 ms per tick, 255 ticks = 4.08 s
static volatile uint16_t ticks_since_last_event = 0;

// maximum number of events which can be waiting at one time
// (would probably be okay to reduce this to 4, but it's higher to be safe)
#define EMISSION_QUEUE_LEN 16
// was "volatile" before, changed to regular var since IRQ rewrites seem
// to have removed the need for it to be volatile
// no comment about "volatile emissions"
Emission emissions[EMISSION_QUEUE_LEN];

void append_emission(Event event, uint16_t arg);
void delete_first_emission();
void process_emissions();
uint8_t emit_now(Event event, uint16_t arg);
void emit(Event event, uint16_t arg);
void emit_current_event(uint16_t arg);
void empty_event_sequence();
uint8_t push_event(uint8_t ev_type);  // only for use by PCINT_inner()


// TODO: Maybe move these to their own file...
// ... this probably isn't the right place for delays.
#ifndef DELAY_FACTOR
    // adjust the timing of delays, lower = shorter delays
    // 90 = 90% delay, 10% for other things
    #define DELAY_FACTOR 92
#endif
inline void interrupt_nice_delays();
uint8_t nice_delay_ms(uint16_t ms);
//uint8_t nice_delay_s();
void delay_4ms(uint8_t ms);


/* Event structure
 * Bit  7: 1 for a button input event, 0 for all others.
 * If bit 7 is 1:
 *     Bits 0,1,2,3:  Click counter.  Up to 15 clicks.
 *     Bit  4: 1 for a "press" event, 0 for a "release" event.
 *     Bit  5: 1 for a "hold" event, 0 otherwise.
 *     Bit  6: 1 for a "timeout" event, 0 otherwise.
 * If bit 7 is 0:
 *     Sort of ad-hoc, shown in #defines below.
 */

// event masks / bits
#define B_SYSTEM               0b00000000
#define B_CLICK                0b10000000
#define B_TIMEOUT              0b01000000
#define B_HOLD                 0b00100000
#define B_PRESS                0b00010000
#define B_RELEASE              0b00000000
#define B_COUNT                0b00001111
#define B_FLAGS                0b11110000

// Event types
#define EV_none                0

// Events which aren't button presses
#define EV_debug               (B_SYSTEM|0b01111111)
#define EV_enter_state         (B_SYSTEM|0b00001000)
#define EV_leave_state         (B_SYSTEM|0b00001001)
#define EV_reenter_state       (B_SYSTEM|0b00001010)
#define EV_tick                (B_SYSTEM|0b00000001)
#ifdef TICK_DURING_STANDBY
#define EV_sleep_tick          (B_SYSTEM|0b00000011)
#endif
#ifdef USE_LVP
#define EV_voltage_low         (B_SYSTEM|0b00000100)
#endif
#ifdef USE_THERMAL_REGULATION
#define EV_temperature_high    (B_SYSTEM|0b00000101)
#define EV_temperature_low     (B_SYSTEM|0b00000110)
#define EV_temperature_okay    (B_SYSTEM|0b00000111)
#endif

// Button press events

// shouldn't normally happen, but UI might empty_event_sequence() while button
// is down so a release with no recorded prior hold could be possible
#define EV_release             (B_CLICK|B_RELEASE|0)

#define EV_click1_press        (B_CLICK|B_PRESS|1)
#define EV_click1_release      (B_CLICK|B_RELEASE|1)
#define EV_click1_complete     (B_CLICK|B_TIMEOUT|1)
#define EV_1click EV_click1_complete
#define EV_click1_hold         (B_CLICK|B_HOLD|B_PRESS|1)
#define EV_click1_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|1)
#define EV_hold EV_click1_hold

#define EV_click2_press        (B_CLICK|B_PRESS|2)
#define EV_click2_release      (B_CLICK|B_RELEASE|2)
#define EV_click2_complete     (B_CLICK|B_TIMEOUT|2)
#define EV_2clicks EV_click2_complete
#define EV_click2_hold         (B_CLICK|B_HOLD|B_PRESS|2)
#define EV_click2_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|2)

#define EV_click3_press        (B_CLICK|B_PRESS|3)
#define EV_click3_release      (B_CLICK|B_RELEASE|3)
#define EV_click3_complete     (B_CLICK|B_TIMEOUT|3)
#define EV_3clicks EV_click3_complete
#define EV_click3_hold         (B_CLICK|B_HOLD|B_PRESS|3)
#define EV_click3_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|3)

#define EV_click4_press        (B_CLICK|B_PRESS|4)
#define EV_click4_release      (B_CLICK|B_RELEASE|4)
#define EV_click4_complete     (B_CLICK|B_TIMEOUT|4)
#define EV_4clicks EV_click4_complete
#define EV_click4_hold         (B_CLICK|B_HOLD|B_PRESS|4)
#define EV_click4_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|4)

#define EV_click5_press        (B_CLICK|B_PRESS|5)
#define EV_click5_release      (B_CLICK|B_RELEASE|5)
#define EV_click5_complete     (B_CLICK|B_TIMEOUT|5)
#define EV_5clicks EV_click5_complete
#define EV_click5_hold         (B_CLICK|B_HOLD|B_PRESS|5)
#define EV_click5_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|5)

#define EV_click6_press        (B_CLICK|B_PRESS|6)
#define EV_click6_release      (B_CLICK|B_RELEASE|6)
#define EV_click6_complete     (B_CLICK|B_TIMEOUT|6)
#define EV_6clicks EV_click6_complete
#define EV_click6_hold         (B_CLICK|B_HOLD|B_PRESS|6)
#define EV_click6_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|6)

#define EV_click7_press        (B_CLICK|B_PRESS|7)
#define EV_click7_release      (B_CLICK|B_RELEASE|7)
#define EV_click7_complete     (B_CLICK|B_TIMEOUT|7)
#define EV_7clicks EV_click7_complete
#define EV_click7_hold         (B_CLICK|B_HOLD|B_PRESS|7)
#define EV_click7_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|7)

#define EV_click8_press        (B_CLICK|B_PRESS|8)
#define EV_click8_release      (B_CLICK|B_RELEASE|8)
#define EV_click8_complete     (B_CLICK|B_TIMEOUT|8)
#define EV_8clicks EV_click8_complete
#define EV_click8_hold         (B_CLICK|B_HOLD|B_PRESS|8)
#define EV_click8_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|8)

#define EV_click9_press        (B_CLICK|B_PRESS|9)
#define EV_click9_release      (B_CLICK|B_RELEASE|9)
#define EV_click9_complete     (B_CLICK|B_TIMEOUT|9)
#define EV_9clicks EV_click9_complete
#define EV_click9_hold         (B_CLICK|B_HOLD|B_PRESS|9)
#define EV_click9_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|9)

#define EV_click10_press        (B_CLICK|B_PRESS|10)
#define EV_click10_release      (B_CLICK|B_RELEASE|10)
#define EV_click10_complete     (B_CLICK|B_TIMEOUT|10)
#define EV_10clicks EV_click10_complete
#define EV_click10_hold         (B_CLICK|B_HOLD|B_PRESS|10)
#define EV_click10_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|10)

#define EV_click11_press        (B_CLICK|B_PRESS|11)
#define EV_click11_release      (B_CLICK|B_RELEASE|11)
#define EV_click11_complete     (B_CLICK|B_TIMEOUT|11)
#define EV_11clicks EV_click11_complete
#define EV_click11_hold         (B_CLICK|B_HOLD|B_PRESS|11)
#define EV_click11_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|11)

#define EV_click12_press        (B_CLICK|B_PRESS|12)
#define EV_click12_release      (B_CLICK|B_RELEASE|12)
#define EV_click12_complete     (B_CLICK|B_TIMEOUT|12)
#define EV_12clicks EV_click12_complete
#define EV_click12_hold         (B_CLICK|B_HOLD|B_PRESS|12)
#define EV_click12_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|12)

#define EV_click13_press        (B_CLICK|B_PRESS|13)
#define EV_click13_release      (B_CLICK|B_RELEASE|13)
#define EV_click13_complete     (B_CLICK|B_TIMEOUT|13)
#define EV_13clicks EV_click13_complete
#define EV_click13_hold         (B_CLICK|B_HOLD|B_PRESS|13)
#define EV_click13_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|13)

#define EV_click14_press        (B_CLICK|B_PRESS|14)
#define EV_click14_release      (B_CLICK|B_RELEASE|14)
#define EV_click14_complete     (B_CLICK|B_TIMEOUT|14)
#define EV_14clicks EV_click14_complete
#define EV_click14_hold         (B_CLICK|B_HOLD|B_PRESS|14)
#define EV_click14_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|14)

#define EV_click15_press        (B_CLICK|B_PRESS|15)
#define EV_click15_release      (B_CLICK|B_RELEASE|15)
#define EV_click15_complete     (B_CLICK|B_TIMEOUT|15)
#define EV_15clicks EV_click15_complete
#define EV_click15_hold         (B_CLICK|B_HOLD|B_PRESS|15)
#define EV_click15_hold_release (B_CLICK|B_HOLD|B_RELEASE|B_TIMEOUT|15)

