#!/bin/bash
# Anduril / FSM build script
# Copyright (C) 2014-2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

# Instead of using a Makefile, since most of the firmwares here build in the
# same exact way, here's a script to do the same thing

if [ 0 = "$#" ]; then
  echo "Usage: build.sh TARGET"
  echo "Example: build.sh hw/hank/emisar-d4/anduril.h"
  exit
fi

# repo root dir
BASEDIR=$(dirname $(dirname "$0"))

TARGET=$1 ; shift
UI=$(basename $TARGET .h)
MODEL=$(dirname $TARGET)
PROGRAM="ui/$UI/$UI"
USER_MODEL_CFG=$(dirname ${TARGET//hw/users\/$USER} )/config.h
USER_DEFAULT_CFG=users/$USER/config.h

# figure out the model number
MODEL_NUMBER=$(head -1 $MODEL/model)

# figure out the MCU type and set some vars
eval $( bin/detect-mcu.sh "$TARGET" )

# detect and enable a relevant Atmel DFP
if [[ $MCUNAME =~ "attiny" ]]; then
  DFPPATH=$BASEDIR/arch/dfp/attiny
elif [[ $MCUNAME =~ "avr" && $MCUNAME =~ "dd" ]]; then
  DFPPATH=$BASEDIR/arch/dfp/avrdd
else
  echo "Unrecognized MCU type: '$MCUNAME'"
  exit 1
fi
# ensure the DFP files exist
if [ ! -d "$DFPPATH" ]; then
  echo "Atmel DFP files not found: '$DFPPATH'"
  echo "Install DFP files with './make dfp'"
  exit 1
fi

export CC=avr-gcc
export CPP=avr-cpp
export OBJCOPY=avr-objcopy
export DFPFLAGS="-B $DFPPATH/gcc/dev/$MCUNAME/ -I $DFPPATH/include/"
INCLUDES="-I ui -I hw -I. -I.. -I../.. -I../../.."
export CFLAGS="  -Wall -g -Os -mmcu=$MCUNAME -c -std=gnu99 -fgnu89-inline -fwhole-program $MCUFLAGS $INCLUDES -fshort-enums $DFPFLAGS"
export CPPFLAGS="-Wall -g -Os -mmcu=$MCUNAME -C -std=gnu99 -fgnu89-inline -fwhole-program $MCUFLAGS $INCLUDES -fshort-enums $DFPFLAGS"
export OFLAGS="-Wall -g -Os -mmcu=$MCUNAME -mrelax $DFPFLAGS"
export LDFLAGS="-fgnu89-inline"
export OBJCOPYFLAGS='--set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex --remove-section .fuse'
export OBJS=$PROGRAM.o

OTHERFLAGS="-DCFG_H=$TARGET -DMODEL_NUMBER=\"$MODEL_NUMBER\""
for arg in "$*" ; do
  OTHERFLAGS="$OTHERFLAGS $arg"
done

if [ -f $USER_DEFAULT_CFG ]; then
  echo "  Using custom configuration from $USER_DEFAULT_CFG"
  OTHERFLAGS="$OTHERFLAGS -DUSER_DEFAULT_H=$USER_DEFAULT_CFG"
fi

if [ -f $USER_MODEL_CFG ]; then
  echo "  Using custom configuration from $USER_MODEL_CFG"
  OTHERFLAGS="$OTHERFLAGS -DUSER_MODEL_H=$USER_MODEL_CFG"
fi

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
#run avr-size -C --mcu=$MCUNAME $PROGRAM.elf | grep Full
run avr-objdump -Pmem-usage $PROGRAM.elf | grep -E 'Full|Device' | sed 's/^/  /;'
