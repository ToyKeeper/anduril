// battcheck-mode-fsm.h: FSM config for battery check mode in Anduril.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#define USE_BATTCHECK

#ifdef USE_AUX_RGB_LEDS
    // show voltage colors for a few seconds after going to standby
    #define USE_POST_OFF_VOLTAGE
    #ifndef DEFAULT_POST_OFF_VOLTAGE_SECONDS
        #define DEFAULT_POST_OFF_VOLTAGE_SECONDS  4
    #endif
    #ifndef POST_OFF_VOLTAGE_BRIGHTNESS
        // level at which to switch from low to high aux brightness
        #define POST_OFF_VOLTAGE_BRIGHTNESS (RAMP_SIZE/10)
    #endif
#endif
