// Noctigon K9.3 config options for Anduril
// Copyright (C) 2020-2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// same as a K9.3, but with a few small changes
#include "hank/noctigon-k9.3/anduril.h"

// channel modes...
// CM_CH1, CM_CH2, CM_BOTH, CM_BLEND, CM_AUTO
// ch1=flood, ch2=throw
// enable max brightness out of the box
#undef DEFAULT_CHANNEL_MODE
#define DEFAULT_CHANNEL_MODE           CM_CH2

// flood and throw only, by default
#undef CHANNEL_MODES_ENABLED
#define CHANNEL_MODES_ENABLED  0b0000000000000011
// _, _, _, 128=equal flood/throw, 255=low-throw-to-high-flood
#undef CHANNEL_MODE_ARGS
#define CHANNEL_MODE_ARGS     0,0,0,128,255,AUXRGB_CM_ARGS

// blink numbers on the main LEDs by default (but allow user to change it)
#undef DEFAULT_BLINK_CHANNEL
#define DEFAULT_BLINK_CHANNEL  CM_BOTH

#undef POLICE_COLOR_STROBE_CH1
#undef POLICE_COLOR_STROBE_CH2
#define POLICE_COLOR_STROBE_CH1        CM_AUXRED
#define POLICE_COLOR_STROBE_CH2        CM_AUXBLU

// party strobe, tac strobe, police, lightning, candle, bike
#undef DEFAULT_STROBE_CHANNELS
#define DEFAULT_STROBE_CHANNELS  CM_CH1,CM_CH2,CM_AUTO,CM_AUTO,CM_CH2,CM_CH1

