#!/usr/bin/env bash

# Flashes both fuses and firmware.

if [ -z "$1" ]; then
  echo "Usage: flash.sh MCU myprogram[.hex]"
  echo "MCU is a number, like '13' for attiny13 or '841' for attiny841"
  echo "The .hex suffix is optional."
  exit
fi

set -e

BASEDIR="$(dirname "$0")"
export ATTINY="$1" ; shift
export PROGRAM="$1" ; shift
if [ ! -f "$PROGRAM" ]; then PROGRAM="$PROGRAM".hex ; fi
"$BASEDIR"/flash-tiny"$ATTINY"-fuses.sh
"$BASEDIR"/flash-tiny"$ATTINY".sh "$PROGRAM"
