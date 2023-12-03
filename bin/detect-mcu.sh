#!/usr/bin/env bash
# Anduril / FSM MCU type detection script
# Copyright (C) 2014-2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

if [ -z "$1" ]; then
  echo "Usage: detect-mcu.sh FILE"
  echo 'or: eval $(detect-mcu.sh FILE)'
  echo "Example: detect-mcu.sh hw/hank/emisar-d4/anduril.h"
  echo "Figures out which MCU a build uses, and sets env vars for it"
  exit
fi

TARGET=$1
while [ -n "$TARGET" ]; do
  #echo "... $TARGET"
  if [ -f "$TARGET" ]; then  # use the dir/$UI.h file
    ATTINY=$(grep 'ATTINY:' "$TARGET" | awk '{ print $3 }')
    if [ -n "$ATTINY" ]; then
      echo "export MCUNAME=attiny${ATTINY}"
      echo "export MCU=0x${ATTINY}"
      echo "export ATTINY=${ATTINY}"
      echo "export MCUFLAGS=\"-DMCUNAME=attiny${ATTINY} -DMCU=0x${ATTINY} -DATTINY=${ATTINY}\""
      exit
    fi
  else  # use the dir/arch file
    if [ -f "${TARGET}/arch" ]; then
      MCU=$(head -1 "${TARGET}/arch")
      if [ -n "$MCU" ]; then
        NUM=$( echo "$MCU" | sed 's/^avr//; s/^attiny//;' )
        echo "export MCUNAME=${MCU}"
        echo "export MCU=0x${NUM}"
        [[ "$MCU" =~ "attiny" ]] && echo "export ATTINY=${NUM}"
        echo "export MCUFLAGS=\"-DMCUNAME=${MCU} -DMCU=0x${NUM} -DATTINY=${NUM}\""
        exit
      fi
    fi
  fi
  # move up one dir
  # if target doesn't change here, exit to avoid infinite loop
  FOO="$TARGET"
  TARGET=$(dirname "$TARGET")
  [ "$FOO" = "$TARGET" ] && exit 1
done

