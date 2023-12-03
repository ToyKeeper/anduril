#!/usr/bin/env sh
FIRMWARE="$1"
avrdude -c usbasp -p t85 -u -Uflash:w:"$FIRMWARE"
