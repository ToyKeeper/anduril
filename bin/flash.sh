#!/bin/bash

# Flashes both fuses and firmware.

set -e

BASEDIR=$(dirname "$0")
export ATTINY=$1 ; shift
export PROGRAM=$1 ; shift
if [ ! -f "$PROGRAM" ]; then PROGRAM="$PROGRAM".hex ; fi
$BASEDIR/flash-tiny"$ATTINY"-fuses.sh
$BASEDIR/flash-tiny"$ATTINY".sh "$PROGRAM"
