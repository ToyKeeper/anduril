// beacontower-mode.c: Beacontower mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
//               2019-2023 loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 Beacon-Tower mode:

    Beacontower mimics the gradual turn-on and fade of large red 
    incandescent beacon bulbs found on obstruction towers. Access 
    this mode from the blinky group in this order: Batt Check, 
    Temp Check, Beacon, SOS, Beacontower. The brightness will be 
    the last memorized level, and defaults at the FAA standard 
    of 30 flashes-per-minute (FPM). 

       - Brightness is the user's last-ramped level. Set this in 
         ramping mode before starting beacon mode.

       - FPM is configured by holding the button. The light will blink
         once per second to aid in counting. Release it after the 
         desired amount of time has passed to set a new flashing period.

         For example, to set a 4-second (15FPM) interval , hold the button 
         for 4 seconds (4 blinks).

         Suppose the period of the light is set as t seconds, given a ramp
         level of ~60/150..
         
         - Turn on ramp = ~1/16 t (depends on ramp level)
         - On at full brightness = 4/16 t 
         - Turn off ramp = ~3/16 t (depends on ramp level)
         - Off = 7/16 t
         
         Increasing the brightness also increases the time taken to ramp up
         and down, just like an incandescent would.

 Access from blinky / utility modes; the sequence is:

  - Battery check.
  - Temperature check (if light has a temperature sensor).
  - Beacon mode.
  - SOS mode (if enabled).
  - Beacon Tower mode (if enabled).
  
*/

#pragma once

#include "anduril/beacontower-mode.h"

inline void beacontower_mode_iter() {
    // one iteration of main loop()
    // TODO: currently does not respond to button during ramp up/downs
    if (! button_last_state) {
    	for (uint8_t i =0; i<memorized_level;i+=2){
    		set_level(i);
    		nice_delay_ms(2); 
    	}
    	set_level(memorized_level);
        nice_delay_ms(beacontower_seconds*250);
		for (uint8_t i  = memorized_level;i>0;i--){
    		set_level(i);
            if (i>30) {nice_delay_ms(5);}
            else {nice_delay_ms(8);}
    	}
        set_level(0);
        nice_delay_ms(beacontower_seconds*480);
    }
}

uint8_t beacontower_state(Event event, uint16_t arg) {
    // 1 click: off
    if (event == EV_1click) {
        set_state(off_state, 0);
        return EVENT_HANDLED;
    }
    // TODO: use sleep ticks to measure time between pulses,
    //       to save power

    // 2 clicks: next blinky mode
    else if (event == EV_2clicks) {
    	#if defined(USE_BATTCHECK_MODE)
        set_state(battcheck_state, 0);
        #elif defined(USE_THERMAL_REGULATION)
        set_state(tempcheck_state, 0);
        #elif defined(USE_BEACON_MODE)
        set_state(beacon_state, 0);
        #elif defined(USE_SOS_MODE) && defined(USE_SOS_MODE_IN_BLINKY_GROUP)
        set_state(sos_state, 0);
        #endif
        return EVENT_HANDLED;
    }
    // hold: configure beacon tower timing
    else if (event == EV_click1_hold) {
        if (0 == (arg % TICKS_PER_SECOND)) {
            blink_once();
        }
        return EVENT_HANDLED;
    }
    // release hold: save new timing
    else if (event == EV_click1_hold_release) {
        beacontower_seconds = 1 + (arg / TICKS_PER_SECOND);
        save_config();
        // TODO: currently does not save beacontower_seconds
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}