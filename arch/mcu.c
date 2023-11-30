// arch/mcu.c: Attiny portability header.
// Copyright (C) 2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "arch/mcu.h"

#define MCU_C arch/MCUNAME.c
#include incfile(MCU_C)

