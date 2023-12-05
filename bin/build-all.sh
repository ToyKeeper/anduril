#!/usr/bin/env bash
# Anduril / FSM build wrapper script
# Copyright (C) 2017-2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

# Usage: build-all.sh [pattern]
# If pattern given, only build targets which match.

# enable "**" for recursive glob (requires bash)
shopt -s globstar

function main {

    if [ "$#" -gt 0 ]; then
        # multiple search terms with "AND"
        SEARCH=( "$@" )
        # memes
        [ "$1" = "me" ] && shift && shift && echo "Make your own $*." && exit 1
    fi

    # TODO: detect UI from $0 and/or $*
    UI=anduril

    mkdir -p hex

    make-version-h  # generate a version.h file

    PASS=0
    FAIL=0
    PASSED=''
    FAILED=''

    # build targets are hw/$vendor/$model/**/$ui.h
    for TARGET in hw/*/*/**/"$UI".h ; do

        # friendly name for this build
        NAME=$(echo "$TARGET" | perl -ne 's|/|-|g; /hw-(.*)-'"$UI"'.h/ && print "$1\n";')

        # limit builds to searched patterns, if given
        SKIP=0
        if [ ${#SEARCH[@]} -gt 0 ]; then
            for text in "${SEARCH[@]}" ; do
                if ! echo "$NAME $TARGET" | grep -i -- "$text" > /dev/null ; then
                    SKIP=1
                fi
            done
        fi
        if [ 1 = $SKIP ]; then continue ; fi

        # announce what we're going to build
        echo "===== $UI $REV : $NAME ====="

        # try to compile, track result, and rename compiled files
        if bin/build.sh "$TARGET" ; then
            HEX_OUT="hex/$UI.$NAME.hex"
            mv -f "ui/$UI/$UI".hex "$HEX_OUT"
            MD5=$(md5sum "$HEX_OUT" | cut -d ' ' -f 1)
            echo "  # $MD5"
            echo "  > $HEX_OUT"
            PASS=$((PASS + 1))
            PASSED="$PASSED $NAME"
        else
            echo "ERROR: build failed"
            FAIL=$((FAIL + 1))
            FAILED="$FAILED $NAME"
        fi

    done

    # summary
    echo "===== $PASS builds succeeded, $FAIL failed ====="
    #echo "PASS: $PASSED"
    if [ 0 != $FAIL ]; then
        echo "FAIL:$FAILED"
        exit 1
    fi
}

function make-version-h {
    # old: version = build date
    #date '+#define VERSION_NUMBER "%Y-%m-%d"' > ui/$UI/version.h

    REV=$(bin/version-string.sh c)
    # save the version name to version.h
    mkdir -p ".build/$UI"
    echo '#define VERSION_NUMBER "'"$REV"'"' > ".build/$UI/version.h"
}

main "$@"

