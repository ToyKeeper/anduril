#!/usr/bin/env sh
# 8 MHz, 64ms boot delay, enable flashing
# (everything else disabled)
# Use low fuse 0xD2 for 4ms startup delay,
#           or 0xE2 for 64ms (useful on a twisty light)
# Use high fuse 0xDE for BOD 1.8V,
#            or 0xDF for no BOD
avrdude -c usbasp -p t1634 -u -U lfuse:w:0xe2:m -U hfuse:w:0xde:m -U efuse:w:0xff:m

