#!/usr/bin/env bash
# AVR firmware flashing script, a wrapper for other tools
# Copyright (C) 2023-2026 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

# Usage: flash-MCUTYPE.sh hex/foo.hex
# (where the script name specifies the type of MCU chip,
#  and the first parameter is the path to a .hex file)
# Example:
#   ./bin/flash-avr32dd20.sh hex/anduril.thefreeman-avr32dd20-devkit.hex

set -e

FLASHPROG='avrdude'
#FLASHPROG='pymcuprog'

# Get the path to a .hex file
[[ -z "$1" ]] && echo "No .hex file specified." && exit 1
HEX="$1"

# figure out the MCU type...
# TODO: find the relevant hw/*/arch file and use that to get MCU type
# use $2 if it exists, and use the name of this script maybe
MCUTYPE="unset"
#MCUTYPE=$(echo "$0" | sed 's/.*flash-\(.*\).sh.*/\1/')
[[ "$0" =~ flash-(.*).sh ]] && MCUTYPE="${BASH_REMATCH[1]}"
[[ -n "$2" ]] && MCUTYPE="$2"

# assume the highest-numbered USB serial device is the correct one,
# since it was probably the most recently plugged in
TTYUSB=$(ls -1 /dev/tty* | grep -i usb | tail -1)

# set options per MCU type
case "$MCUTYPE" in
    t1616|avr32dd20)
        PROTO='serialupdi'
        TTYARGS="-P $TTYUSB"
        ;;
    t25|t45|t85|t1634)
        PROTO='usbasp'
        TTYUSB='auto'
        TTYARGS=''
        ;;
    *)
        echo 'Error: Unknown MCU type.'
        exit 1
        ;;
esac

# Do the actual flashing
echo "Flashing $MCUTYPE MCU on port $TTYUSB: $HEX"

case "$FLASHPROG" in

    avrdude)
        avrdude \
            -c "$PROTO" \
            -p "$MCUTYPE" \
            $TTYARGS \
            -u -Uflash:w:"$HEX"
        ;;

    pymcuprog)
        pymcuprog write \
            --erase \
            --verify \
            --timing \
            -t uart \
            -u "$TTYUSB" \
            -d "$MCUTYPE" \
            -f "$HEX"
        ;;

    *)
        echo 'Error: Flashing program must be "avrdude" or "pymcuprog".'
        exit 1
        ;;

esac

