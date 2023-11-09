#!/usr/bin/env sh
FIRMWARE="$1"
if [ -z "$AVRDUDE_TTYUSB" ]; then AVRDUDE_TTYUSB=/dev/ttyUSB0 ; fi

# In your shell config:
# from https://github.com/ElTangas/jtag2updi ...
# export AVRDUDE_CONF="-C$HOME/path/to/jtag2updi/avrdude.conf"
# export AVRDUDE_TTYUSB="/dev/ttyUSB2"

avrdude "$AVRDUDE_CONF" \
  -c jtag2updi \
  -P "$AVRDUDE_TTYUSB" \
  -p t1616 \
  -u -Uflash:w:"$FIRMWARE"
