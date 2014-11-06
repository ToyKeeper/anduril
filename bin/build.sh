#!/bin/bash

# Instead of using a Makefile, since most of the firmwares here build in the
# same exact way, here's a script to do the same thing

export PROGRAM=$1
export MCU=attiny13
export CC=avr-gcc
export OBJCOPY=avr-objcopy
export CFLAGS="-Wall -g -Os -mmcu=$MCU"
export LDFLAGS=
export OBJCOPYFLAGS='--set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex'
export OBJS=$PROGRAM.o

function run () {
  echo $*
  $*
  if [ x"$?" != x0 ]; then exit 1 ; fi
}

run $CC $CFLAGS -o $PROGRAM.o -c $PROGRAM.c
run $CC $CFLAGS $LDFLAGS -o $PROGRAM.elf $PROGRAM.o
run $OBJCOPY $OBJCOPYFLAGS $PROGRAM.elf $PROGRAM.hex
run avr-size $PROGRAM.hex
