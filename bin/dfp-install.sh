#!/usr/bin/env sh
# Atmel DFP download/install/setup script
# Copyright (C) 2023-2026 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

URL_ATMEL="http://packs.download.atmel.com/"
URL_MCHIP="https://packs.download.microchip.com/"

# attiny1634 / attiny85
# attiny1634 won't boot with the newer DFPs, thanks Microchip/Atmel
# 3.1.260 (2023-08-16) is the last version which works
#V_ATTINY='2.0.368'  # works, 2022-03-02
#V_ATTINY='2.1.484'  # fails, 2026-04-11
#F_ATTINY="Atmel.ATtiny_DFP.${V_ATTINY}.atpack"
#URL_ATTINY="${URL_ATMEL}${F_ATTINY}"
V_ATTINY='3.1.260'  # works, 2023-08-16
#V_ATTINY='3.2.268'  # fails, 2025-01-13
F_ATTINY="Microchip.ATtiny_DFP.${V_ATTINY}.atpack"
URL_ATTINY="${URL_MCHIP}${F_ATTINY}"

# attiny1616
V_ATTINY1616='3.4.278'
F_ATTINY1616="Microchip.ATtiny_DFP.${V_ATTINY1616}.atpack"
URL_ATTINY1616="${URL_MCHIP}${F_ATTINY1616}"

# avr32dd20
#V_AVRDD='2.2.253'  # works, 2022-12-16
#F_AVRDD="Atmel.AVR-Dx_DFP.${V_AVRDD}.atpack"
#URL_AVRDD="${URL_ATMEL}${F_AVRDD}"
V_AVRDD='2.8.343'  # works, 2026-04-11 (makes same binaries as 2.2.253)
F_AVRDD="Microchip.AVR-Dx_DFP.${V_AVRDD}.atpack"
URL_AVRDD="${URL_MCHIP}${F_AVRDD}"


cd arch

# make the base DFP dir
mkdir -p dfp
cd dfp

# download the zipped pack files if they don't exist yet
mkdir -p zip
cd zip
[ ! -f "${F_ATTINY}"     ] && wget "${URL_ATTINY}"
[ ! -f "${F_ATTINY1616}" ] && wget "${URL_ATTINY1616}"
[ ! -f "${F_AVRDD}"      ] && wget "${URL_AVRDD}"

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

