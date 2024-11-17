// Copyright (C) 2017-2023 Selene ToyKeeper
//               2021-2024 loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later

//***********************************************
//**  HELPER FUNCTIONS FOR LUME-X1-AVR32DD20   **
//***********************************************

#pragma once

#include "fsm/chan-rgbaux.c"

// Declare variables and functions to support UDR multiple power paths
uint8_t power_path_level = 0;        // 0 = off, 1 = firefly, 2 = low, 3 = high
uint8_t is_boost_currently_on = 0;   // for turn-on delay during first turn on

void set_level_udr(uint8_t level);
void set_power_path(uint8_t ramp_level);
void set_level_zero();
bool gradual_tick_main(uint8_t gt);

Channel channels[] = {
    { // main LEDs
        .set_level    = set_level_udr,
        .gradual_tick = gradual_tick_main
    },
    RGB_AUX_CHANNELS
};

// turn off
void set_level_zero() {

    power_path_level = 0;   // set power path level to 0
    DAC_LVL  = 0;           // set DAC to 0
    DAC_VREF = V10;         // set DAC Vref to lowest 

    // turn off DC/DC converter and amplifier 
    BST_ENABLE_PORT &= ~(1 << BST_ENABLE_PIN);
    is_boost_currently_on = 0;

    // turn off all UDR paths
    LED_PATH1_PORT &= ~LED_PATH1_PIN;   
    LED_PATH2_PORT &= ~LED_PATH2_PIN;
    LED_PATH3_PORT &= ~LED_PATH3_PIN;
}

// UDR for set_level, which sets the led brightness based on ramp tables.
// single set of LED(s), fully regulated boost at all levels
void set_level_udr(uint8_t level) {
    if (level == actual_level - 1) return;  //  no-op

    // get the ramp data
    PWM1_DATATYPE dac_lvl  = PWM1_GET(level) << 6;  // dac register is left-aligned
    PWM2_DATATYPE dac_vref = PWM2_GET(level);

    if(is_boost_currently_on != 1){
        // boost is not on, enable buck and add boot-up delay
        is_boost_currently_on = 1;
        BST_ENABLE_PORT |= (1 << BST_ENABLE_PIN);   // turn on buck and amplifier
        delay_4ms(BST_ON_DELAY/4);                  // boot-up delay
    }
    // set the DAC
    DAC_LVL  = dac_lvl;
    DAC_VREF = dac_vref;

    // set the power paths
    set_power_path(level);
}

// handles dynamic Vref used in the ramp tables 
bool gradual_tick_main(uint8_t gt) {
    // TODO overall smoothness can be improved due to gt using linear
    // adjustments, but ramp table is non-linear.

    // if Vref is the same, make gradual adjustments. 
    // else, jump to the next ramp level and use set_level() to handle power paths.
    PWM2_DATATYPE vref_next = PWM2_GET(gt); // DAC ramp table Vref

    // if different vref level, make a ramp level adjustment..
    if (vref_next != DAC_VREF) return true;  // use set_level() to handle normally

    // .. else, same vref, adjust level gradually. 
    PWM1_DATATYPE dac_next  = PWM1_GET(gt); // DAC ramp table data
    PWM1_DATATYPE dac_curr  = DAC_LVL >> 6; // register is left-aligned

    GRADUAL_ADJUST_SIMPLE(dac_next, dac_curr);
    DAC_LVL = dac_curr << 6;
    
    if (dac_next == dac_curr) return true;  // done

    return false;
}

// handles dynamic power pathways based on threshold levels  
void set_power_path(uint8_t ramp_level){

    ramp_level++; // convert to 1-based indexing

    // check if the ramp level corresponds to a new power path or not;
    // if it doesn't, no need to change anything.
    if (ramp_level>=LED_PATH3_PIN_LEVEL_MIN){
        if (power_path_level != 3){
            // high Mode
            LED_PATH1_PORT |= LED_PATH1_PIN;
            LED_PATH2_PORT |= LED_PATH2_PIN;
            LED_PATH3_PORT |= LED_PATH3_PIN;
            power_path_level = 3;
        }
    }
    else if (ramp_level>=LED_PATH2_PIN_LEVEL_MIN){
        if (power_path_level != 2){
            // low Mode
            //LED_PATH1_PORT &= ~LED_PATH1_PIN;
            LED_PATH1_PORT |= LED_PATH1_PIN;
            LED_PATH2_PORT |= LED_PATH2_PIN;
            LED_PATH3_PORT &= ~LED_PATH3_PIN;
            power_path_level = 2;
        }
    }
    else if (ramp_level>=LED_PATH1_PIN_LEVEL_MIN){
        if (power_path_level != 1){
            // firefly mode
            LED_PATH1_PORT |= LED_PATH1_PIN;
            LED_PATH2_PORT &= ~LED_PATH2_PIN;
            LED_PATH3_PORT &= ~LED_PATH3_PIN;
            power_path_level = 1;
        }
    }
}

