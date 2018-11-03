#/bin/sh
FIRMWARE=$1
avrdude -c usbasp -p t13 -u -Uflash:w:$FIRMWARE
