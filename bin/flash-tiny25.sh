#/bin/sh
FIRMWARE=$1
avrdude -c usbasp -p t25 -u -Uflash:w:$FIRMWARE
