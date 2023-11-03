#!/bin/bash

# Instead of using a Makefile, since most of the firmwares here build in the
# same exact way, here's a script to do the same thing

if [ -z "$1" ]; then
  echo "Usage: build.sh TARGET USER"
  echo "Example: build.sh hw/hank/emisar-d4/anduril.h users/myuser"
  echo "(but USER isn't implemented yet)"
  exit
fi

TARGET=$1 ; shift
UI=$(basename $TARGET .h)

ATTINY=$(grep 'ATTINY:' $TARGET | awk '{ print $3 }')
if [ -z "$ATTINY" ]; then ATTINY=85 ; fi

PROGRAM="ui/$UI/$UI"

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
export CPP=avr-cpp
export OBJCOPY=avr-objcopy
export DFPFLAGS="-B $ATTINY_DFP/gcc/dev/$MCU/ -I $ATTINY_DFP/include/"
# TODO: include $user/ first so it can override other stuff
INCLUDES="-I ui -I hw -I. -I.. -I../.. -I../../.."
export CFLAGS="  -Wall -g -Os -mmcu=$MCU -c -std=gnu99 -fgnu89-inline -fwhole-program -DATTINY=$ATTINY $INCLUDES -fshort-enums $DFPFLAGS"
export CPPFLAGS="-Wall -g -Os -mmcu=$MCU -C -std=gnu99 -fgnu89-inline -fwhole-program -DATTINY=$ATTINY $INCLUDES -fshort-enums $DFPFLAGS"
export OFLAGS="-Wall -g -Os -mmcu=$MCU -mrelax $DFPFLAGS"
export LDFLAGS="-fgnu89-inline"
export OBJCOPYFLAGS='--set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex --remove-section .fuse'
export OBJS=$PROGRAM.o

OTHERFLAGS="-DCFG_H=$TARGET"
for arg in "$*" ; do
  OTHERFLAGS="$OTHERFLAGS $arg"
done

function run () {
  #echo $1 ; shift
  #echo $*
  $*
  if [ x"$?" != x0 ]; then exit 1 ; fi
}

run $CPP $OTHERFLAGS $CPPFLAGS -o foo.cpp $PROGRAM.c
grep -a -E -v '^#|^$' foo.cpp > $PROGRAM.cpp ; rm foo.cpp
run $CC $OTHERFLAGS $CFLAGS -o $PROGRAM.o -c $PROGRAM.c
run $CC $OFLAGS $LDFLAGS -o $PROGRAM.elf $PROGRAM.o
run $OBJCOPY $OBJCOPYFLAGS $PROGRAM.elf $PROGRAM.hex
# deprecated
#run avr-size -C --mcu=$MCU $PROGRAM.elf | grep Full
run avr-objdump -Pmem-usage $PROGRAM.elf | grep -E 'Full|Device' | sed 's/^/  /;'
