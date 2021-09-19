#!/bin/bash

# Instead of using a Makefile, since most of the firmwares here build in the
# same exact way, here's a script to do the same thing

if [ -z "$1" ]; then
  echo "Usage: build.sh MCU myprogram"
  echo "MCU is a number, like '13' for attiny13 or '841' for attiny841"
  exit
fi

export ATTINY=$1 ; shift
export PROGRAM=$1 ; shift

# give a more useful error message when AVR DFP is needed but not installed
# (Atmel ATtiny device family pack, for attiny1616 support)
# http://packs.download.atmel.com/
#if [ -z "$ATTINY_DFP" ]; then export ATTINY_DFP=~/avr/attiny_dfp ; fi
SERIES1=' 416 417 816 817 1616 1617 3216 3217 '
if [[ $SERIES1 =~ " $ATTINY " ]]; then
  if [ -z "$ATTINY_DFP" ]; then
    echo "ATtiny$ATTINY support requires Atmel attiny device family pack."
    echo "More info is in /README under tiny1616 support."
    exit 1
  fi
fi

export MCU=attiny$ATTINY
export CC=avr-gcc
export OBJCOPY=avr-objcopy
export DFPFLAGS="-B $ATTINY_DFP/gcc/dev/$MCU/ -I $ATTINY_DFP/include/"
export CFLAGS="-Wall -g -Os -mmcu=$MCU -c -std=gnu99 -fgnu89-inline -fwhole-program -DATTINY=$ATTINY -I.. -I../.. -I../../.. -fshort-enums $DFPFLAGS"
export OFLAGS="-Wall -g -Os -mmcu=$MCU -mrelax $DFPFLAGS"
export LDFLAGS="-fgnu89-inline"
export OBJCOPYFLAGS='--set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex'
export OBJS=$PROGRAM.o

for arg in "$*" ; do
  OTHERFLAGS="$OTHERFLAGS $arg"
done

function run () {
  echo $*
  $*
  if [ x"$?" != x0 ]; then exit 1 ; fi
}

run $CC $OTHERFLAGS $CFLAGS -o $PROGRAM.o -c $PROGRAM.c
run $CC $OFLAGS $LDFLAGS -o $PROGRAM.elf $PROGRAM.o
run $OBJCOPY $OBJCOPYFLAGS $PROGRAM.elf $PROGRAM.hex
# deprecated
#run avr-size -C --mcu=$MCU $PROGRAM.elf | grep Full
run avr-objdump -Pmem-usage $PROGRAM.elf | grep Full
