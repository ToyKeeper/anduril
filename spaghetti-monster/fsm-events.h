/*
 * fsm-events.h: Event-handling functions for SpaghettiMonster.
 *
 * Copyright (C) 2017 Selene ToyKeeper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FSM_EVENTS_H
#define FSM_EVENTS_H

#include <avr/pgmspace.h>

// typedefs
typedef PROGMEM const uint8_t Event;
typedef Event * EventPtr;
typedef struct Emission {
    EventPtr event;
    uint16_t arg;
} Emission;

#define EVENT_HANDLED 0
#define EVENT_NOT_HANDLED 1
#define MISCHIEF_MANAGED EVENT_HANDLED
#define MISCHIEF_NOT_MANAGED EVENT_NOT_HANDLED

#ifndef MAX_CLICKS
#define MAX_CLICKS 4
#endif

#define EV_MAX_LEN ((MAX_CLICKS*2)+3)
uint8_t current_event[EV_MAX_LEN];
// at 0.016 ms per tick, 255 ticks = 4.08 s
// TODO: 16 bits?
static volatile uint16_t ticks_since_last_event = 0;

// timeout durations in ticks (each tick 1/60th s)
#define HOLD_TIMEOUT 24
#define RELEASE_TIMEOUT 24

#define A_ENTER_STATE     1
#define A_LEAVE_STATE     2
#define A_TICK            3
#define A_PRESS           4
#define A_HOLD            5
#define A_RELEASE         6
#define A_RELEASE_TIMEOUT 7
// TODO: add events for over/under-heat conditions (with parameter for severity)
#define A_OVERHEATING     8
#define A_UNDERHEATING    9
// TODO: add events for low voltage conditions
#define A_VOLTAGE_LOW     10
//#define A_VOLTAGE_CRITICAL 11
#define A_DEBUG           255  // test event for debugging

// Event types
Event EV_debug[] = {
    A_DEBUG,
    0 } ;
Event EV_enter_state[] = {
    A_ENTER_STATE,
    0 } ;
Event EV_leave_state[] = {
    A_LEAVE_STATE,
    0 } ;
Event EV_tick[] = {
    A_TICK,
    0 } ;
#ifdef USE_LVP
Event EV_voltage_low[] = {
    A_VOLTAGE_LOW,
    0 } ;
#endif
#ifdef USE_THERMAL_REGULATION
Event EV_temperature_high[] = {
    A_OVERHEATING,
    0 } ;
Event EV_temperature_low[] = {
    A_UNDERHEATING,
    0 } ;
#endif
Event EV_click1_press[] = {
    A_PRESS,
    0 };
// shouldn't normally happen, but UI might reset event while button is down
// so a release with no recorded prior hold could be possible
Event EV_release[] = {
    A_RELEASE,
    0 };
Event EV_click1_release[] = {
    A_PRESS,
    A_RELEASE,
    0 };
#define EV_1click EV_click1_complete
Event EV_click1_complete[] = {
    A_PRESS,
    A_RELEASE,
    A_RELEASE_TIMEOUT,
    0 };
#define EV_hold EV_click1_hold
// FIXME: Should holds use "start+tick" or just "tick" with a tick number?
//        Or "start+tick" with a tick number?
Event EV_click1_hold[] = {
    A_PRESS,
    A_HOLD,
    0 };
Event EV_click1_hold_release[] = {
    A_PRESS,
    A_HOLD,
    A_RELEASE,
    0 };
#if MAX_CLICKS >= 2
Event EV_click2_press[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    0 };
Event EV_click2_hold[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_HOLD,
    0 };
Event EV_click2_hold_release[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_HOLD,
    A_RELEASE,
    0 };
Event EV_click2_release[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    0 };
#define EV_2clicks EV_click2_complete
Event EV_click2_complete[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_RELEASE_TIMEOUT,
    0 };
#endif  // MAX_CLICKS >= 2
#if MAX_CLICKS >= 3
Event EV_click3_press[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    0 };
Event EV_click3_release[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    0 };
#define EV_3clicks EV_click3_complete
Event EV_click3_complete[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_RELEASE_TIMEOUT,
    0 };
#endif  // MAX_CLICKS >= 3
#if MAX_CLICKS >= 4
#define EV_4clicks EV_click4_complete
Event EV_click4_complete[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_RELEASE_TIMEOUT,
    0 };
#endif
#if MAX_CLICKS >= 5
#define EV_5clicks EV_click5_complete
Event EV_click5_complete[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_RELEASE_TIMEOUT,
    0 };
#endif
#if MAX_CLICKS >= 6
#define EV_6clicks EV_click6_complete
Event EV_click6_complete[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_RELEASE_TIMEOUT,
    0 };
#endif
// ... and so on

// A list of button event types for easy iteration
EventPtr event_sequences[] = {
    EV_click1_press,
    EV_release,
    EV_click1_release,
    EV_click1_complete,
    EV_click1_hold,
    EV_click1_hold_release,
    #if MAX_CLICKS >= 2
    EV_click2_press,
    EV_click2_hold,
    EV_click2_hold_release,
    EV_click2_release,
    EV_click2_complete,
    #endif
    #if MAX_CLICKS >= 3
    EV_click3_press,
    EV_click3_release,
    EV_click3_complete,
    #endif
    #if MAX_CLICKS >= 4
    EV_click4_complete,
    #endif
    #if MAX_CLICKS >= 5
    EV_click5_complete,
    #endif
    #if MAX_CLICKS >= 6
    EV_click6_complete,
    #endif
    // ...
};

#define events_match(a,b) compare_event_sequences(a,b)
// return 1 if (a == b), 0 otherwise
uint8_t compare_event_sequences(uint8_t *a, const uint8_t *b);
void empty_event_sequence();
uint8_t push_event(uint8_t ev_type);
// uint8_t last_event(uint8_t offset);
inline uint8_t last_event_num();


#define EMISSION_QUEUE_LEN 16
// no comment about "volatile emissions"
volatile Emission emissions[EMISSION_QUEUE_LEN];

void append_emission(EventPtr event, uint16_t arg);
void delete_first_emission();
void process_emissions();
//#define emit_now emit
uint8_t emit_now(EventPtr event, uint16_t arg);
void emit(EventPtr event, uint16_t arg);
void emit_current_event(uint16_t arg);

uint8_t nice_delay_ms(uint16_t ms);
//uint8_t nice_delay_4ms(uint8_t ms);
//uint8_t nice_delay_s();
inline void interrupt_nice_delays();

#endif
