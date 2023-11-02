// fsm-main.h: main() function for SpaghettiMonster.
// Copyright (C) 2017-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

int main();
// needs to run frequently to execute the logic for WDT and ADC and stuff
void handle_deferred_interrupts();

