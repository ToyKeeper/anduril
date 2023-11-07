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


// cause a build failure if `condition` is true
// (gcc compile trick taken from an old version of include/linux/kernel.h)
// more info here:
// https://scaryreasoner.wordpress.com/2009/02/28/checking-sizeof-at-compile-time/
#define BUILD_FAIL_IF(name, condition)  \
    __attribute__((unused)) static void name() \
    { ((void)sizeof(char[1 - 2*!!(condition)])); }

// cause a build failure if `condition` is false
#define BUILD_ASSERT(name, condition)  \
    __attribute__((unused)) static void name() \
    { ((void)sizeof(char[1 - 2*!(condition)])); }

