#/bin/sh
FIRMWARE=$1
avrdude -c jtag2updi -P /dev/ttyUSB0 -p t1616 -u -Uflash:w:$FIRMWARE