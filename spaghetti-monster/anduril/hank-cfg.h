// Intl-Outdoor (Hank)'s config options for Anduril
// Copyright (C) 2021-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// config preferences for Hank Wang of Intl-Outdoor (Emisar, Noctigon)

// RGB aux LEDs should use rainbow cycling mode
// to impress new customers
// (people usually change it to voltage mode later though)
#ifdef RGB_LED_OFF_DEFAULT
#undef RGB_LED_OFF_DEFAULT
#endif
#define RGB_LED_OFF_DEFAULT 0x18  // low, rainbow

// half a second per color in rainbow mode
//#define RGB_RAINBOW_SPEED 0x03

// Allow 3C (or 6C) in Simple UI (toggle smooth or stepped ramping)
#define USE_SIMPLE_UI_RAMPING_TOGGLE

// allow Aux Config and Strobe Modes in Simple UI
#define USE_EXTENDED_SIMPLE_UI

// double click while on goes to full-power turbo, not ramp ceiling
#define DEFAULT_2C_STYLE 1

// for consistency with KR4 (not otherwise necessary though)
#define USE_SOFT_FACTORY_RESET

