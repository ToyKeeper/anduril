// tk.h : misc handy stuff
// Copyright (C) 2015-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

/////
// tk.h
// misc tricks which need to be available before other includes,
// but which don't need to be repeated in every source file
////

// AVR GCC has no boolean type by default
// (this isn't native bool, but it's mostly okay)
#define bool uint8_t
#define false 0
#define true 1
#define FALSE 0
#define TRUE 1

// create a way to include files defined at the command line,
// like with "gcc -DCONFIGFILE=foo.h"
#define incfile2(s) #s
#define incfile(s) incfile2(s)
// use it like this:
//#include incfile(CONFIGFILE)

