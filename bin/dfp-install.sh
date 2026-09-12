#!/usr/bin/env sh
# Atmel DFP download/install/setup script
# Copyright (C) 2023-2026 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

# attiny1634 won't boot with the newer Microchip DFP, thanks Microchip
V_ATTINY='2.0.368'
F_ATTINY="Atmel.ATtiny_DFP.${V_ATTINY}.atpack"

V_ATTINY1616='3.4.278'
F_ATTINY1616="Microchip.ATtiny_DFP.${V_ATTINY1616}.atpack"

V_AVRDD='2.8.343'
F_AVRDD="Microchip.AVR-Dx_DFP.${V_AVRDD}.atpack"

OLDURL="http://packs.download.atmel.com/"
BASEURL="https://packs.download.microchip.com/"

cd arch

# make the base DFP dir
mkdir -p dfp
cd dfp

# download the zipped pack files if they don't exist yet
mkdir -p zip
cd zip
[ ! -f "${F_ATTINY}"     ] && wget "${OLDURL}${F_ATTINY}"
[ ! -f "${F_ATTINY1616}" ] && wget "${BASEURL}${F_ATTINY1616}"
[ ! -f "${F_AVRDD}"      ] && wget "${BASEURL}${F_AVRDD}"

# extract the packs

# attiny
cd ..
mkdir -p attiny
cd attiny
unzip "../zip/${F_ATTINY}"

# attiny1616
cd ..
mkdir -p attiny1616
cd attiny1616
unzip "../zip/${F_ATTINY1616}"

# avrdd
cd ..
mkdir -p avrdd
cd avrdd
unzip "../zip/${F_AVRDD}"

