#!/usr/bin/env sh
# Atmel DFP download/install/setup script
# Copyright (C) 2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

V_ATTINY='2.0.368'
V_AVRDD='2.2.253'
F_ATTINY="Atmel.ATtiny_DFP.${V_ATTINY}.atpack"
F_AVRDD="Atmel.AVR-Dx_DFP.${V_AVRDD}.atpack"

cd arch

# make the base DFP dir
mkdir -p dfp
cd dfp

# download the zipped pack files if they don't exist yet
mkdir -p zip
cd zip
[ ! -f "${F_ATTINY}" ] && wget "http://packs.download.atmel.com/${F_ATTINY}"
[ ! -f "${F_AVRDD}"  ] && wget "http://packs.download.atmel.com/${F_AVRDD}"

# extract the packs
# attiny
cd ..
mkdir -p attiny
cd attiny
unzip "../zip/${F_ATTINY}"
# avrdd
cd ..
mkdir -p avrdd
cd avrdd
unzip "../zip/${F_AVRDD}"

