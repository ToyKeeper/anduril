// Noctigon KR4 (12V) config options for Anduril
// (and Noctigon KR1)
// Copyright (C) 2020-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// the only things different from dm11-boost are:
// - e-switch is on a different pin (defined in hwdef)
// - different model number
#include "cfg-noctigon-dm11-boost.h"
#include "hwdef-noctigon-kr4-boost.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0216"
#include "hank-cfg.h"
// ATTINY: 1634

