// Wurkkos config options for Anduril
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// allow Aux Config and Strobe Modes in Simple UI
#define USE_EXTENDED_SIMPLE_UI

// Allow 3C in Simple UI for switching between smooth and stepped ramping
#define USE_SIMPLE_UI_RAMPING_TOGGLE

// enable 2 click turbo (Anduril 1 style)
#define DEFAULT_2C_STYLE 1

// enable SOS in the blinkies group
#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

// enable factory reset on 13H without loosening tailcap
// only really needed on TS10, but doesn't hurt on other models
#define USE_SOFT_FACTORY_RESET

// higher temperature limit because attiny1616 is more accurate
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 50

