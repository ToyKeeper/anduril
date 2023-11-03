#!/bin/sh

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

  # limit builds to searched patterns, if given
  SKIP=0
  if [ ! -z "$SEARCH" ]; then
    for text in $SEARCH ; do
        echo "$TARGET" | grep -i "$text" > /dev/null
        if [ 0 != $? ]; then SKIP=1 ; fi
    done
  fi
  if [ 1 = $SKIP ]; then continue ; fi

  # friendly name for this build
  NAME=$(echo "$TARGET" | perl -ne 's|/|-|g; /hw-(.*)-'"$UI"'.h/ && print "$1\n";')
  echo "===== $UI : $NAME ====="

  # figure out MCU type
  #ATTINY=$(grep 'ATTINY:' $TARGET | awk '{ print $3 }')
  #if [ -z "$ATTINY" ]; then ATTINY=85 ; fi

  # try to compile
  #echo bin/build.sh "$UI" "$TARGET"
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
