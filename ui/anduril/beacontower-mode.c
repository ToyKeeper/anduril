// beacontower-mode.c: Beacontower mode for Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
//               2021-2023 loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 Beacon-Tower mode:

 	Similar to Beacon, but emulates the gradual turn on and off of traditional
    large incandescent bulbs of obstruction/beacon towers. The brightness and 
    the flashes per minutes (FPM) are configurable, with the default being the 
    standard 30 FPM, with roughly 1/4 duty cycle.

       - Brightness is the user's last-ramped level, so set this in 
         ramping mode before starting beacon mode.

       - FPM is configured by holding the button. The light will blink
         once per second while holding the button. Release it after the 
         desired amount of time has passed, to set a new beacon flashing
         interval. 

         For example, to set a 4-second (15FPM) interval , hold the button 
         for 4 seconds (4 blinks).

         Suppose the period of the light is set as t seconds:
         
         - Turn on ramp = ~t/8s
         - On at full brightness = 3t/16s 
         - Turn off ramp = ~t/4s
         - Off = 7t/16s
         
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
    if (! button_last_state) {
    	for (uint8_t i =0; i<memorized_level;i+=2){
    		set_level(i);
    		// nice_delay_ms(2); 
    	}
    	set_level(memorized_level);
        nice_delay_ms(beacontower_seconds*188);
		for (uint8_t i  = memorized_level;i>0;i--){
    		set_level(i);
            if (i>30) {nice_delay_ms(1);}
            else {nice_delay_ms(2);}
    	}
        set_level(0);
        nice_delay_ms(beacontower_seconds*438);
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
        // to save beacontower_seconds? #TODO
        return EVENT_HANDLED;
    }
    return EVENT_NOT_HANDLED;
}