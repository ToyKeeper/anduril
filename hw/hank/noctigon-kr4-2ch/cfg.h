// Noctigon KR4 2-channel config options for Anduril
// Copyright (C) 2021-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// (basically the same as Emisar generic 2-channel build,
// but switch on a different pin, and no lighted button)
// ATTINY: 1634
#include "hwdef-noctigon-kr4-2ch.h"
#include "cfg-emisar-2ch.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0215"

// the button doesn't light up
#undef USE_BUTTON_LED

