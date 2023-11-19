#!/usr/bin/env bash

set -e

[[ -z "$1" ]] && echo "No .hex file specified." && exit 1
HEX="$1"

#pymcuprog ping -t uart -u /dev/ttyUSB2 -d avr32dd20
#pymcuprog erase -t uart -u /dev/ttyUSB2 -d avr32dd20
#pymcuprog write verify -t uart -u /dev/ttyUSB2 -d avr32dd20 -f "$HEX"
#pymcuprog verify -t uart -u /dev/ttyUSB2 -d avr32dd20 -f "$HEX"

pymcuprog write --erase --verify --timing -t uart -u /dev/ttyUSB2 -d avr32dd20 -f "$HEX"
