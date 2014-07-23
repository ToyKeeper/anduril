#/bin/sh
FIRMWARE=$1
avrdude -c usbasp -p t13 -V -u -Uflash:w:$FIRMWARE -Ulfuse:w:0x75:m -Uhfuse:w:0xFF:m
