#!/bin/sh
# Anduril / FSM build wrapper script
# Copyright (C) 2017-2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

# Usage: build-all.sh [pattern]
# If pattern given, only build targets which match.

if [ ! -z "$1" ]; then
  # multiple search terms with "AND"
  SEARCH=$@
fi

# TODO: detect UI from $0 and/or $*
UI=anduril

mkdir -p hex

# TODO: use a git tag for the version, instead of build date
# TODO: use build/version.h instead of $UI/version.h ?
date '+#define VERSION_NUMBER "%Y-%m-%d"' > ui/$UI/version.h

PASS=0
FAIL=0
PASSED=''
FAILED=''

# build targets are hw/*/**/$UI.h
for TARGET in $( find hw/*/*/ -name "$UI".h ) ; do

  # friendly name for this build
  NAME=$(echo "$TARGET" | perl -ne 's|/|-|g; /hw-(.*)-'"$UI"'.h/ && print "$1\n";')

  # limit builds to searched patterns, if given
  SKIP=0
  if [ ! -z "$SEARCH" ]; then
    for text in $SEARCH ; do
        echo "$NAME $TARGET" | grep -i "$text" > /dev/null
        if [ 0 != $? ]; then SKIP=1 ; fi
    done
  fi
  if [ 1 = $SKIP ]; then continue ; fi

  # announce what we're going to build
  echo "===== $UI : $NAME ====="

  # try to compile
  bin/build.sh "$TARGET"

  # track result, and rename compiled files
  if [ 0 = $? ] ; then
    mv -f "ui/$UI/$UI".hex hex/"$UI".$NAME.hex
    echo "  > hex/$UI.$NAME.hex"
    PASS=$(($PASS + 1))
    PASSED="$PASSED $NAME"
  else
    echo "ERROR: build failed"
    FAIL=$(($FAIL + 1))
    FAILED="$FAILED $NAME"
  fi

done

# summary
echo "===== $PASS builds succeeded, $FAIL failed ====="
#echo "PASS: $PASSED"
if [ 0 != $FAIL ]; then
  echo "FAIL:$FAILED"
fi
