#!/usr/bin/env bash
# Create a release file for Anduril .hex files.
# Copyright (C) 2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

set -e  # abort on error

# run from repo root
REPODIR=$(dirname "$0")/..
cd "$REPODIR"

# get rid of 1st arg if it's "release" passed from 'make'
[[ "$1" = "release" ]] && shift

# try to get the repo ready for a release
# (or not; probably better to do these steps manually)
#make clean
#make

# release name
REV=$(bin/version-string.sh git)
# allow manually specifying a release name
[[ -n "$1" ]] && REV="$1"

# optional filter for which files to incude
FILTER=${2:-.}

# releases are named "$project.$revision"
RELNAME="anduril.$REV"

# release directory
RELDIR="releases/$RELNAME"
[[ "." != "$FILTER" ]] && RELDIR="releases/$RELNAME.$FILTER"
mkdir -p "$RELDIR" "$RELDIR/hex"

# add documentation and stuff
cp -a \
    ChangeLog.md \
    LICENSE \
    MODELS \
    README.md \
    docs/anduril-manual.md \
    docs/battery-rainbow.png \
    docs/which-hex-file.md \
    "$RELDIR"

# maybe filter the models list too
[[ "." != "$FILTER" ]] && \
grep -E "$FILTER|^Model |^-----" MODELS > "$RELDIR/MODELS"

# add hardware-specific docs
for f in $(find hw -name '*.md' | grep -i "$FILTER") ; do
    d=$(echo "$f" | sed 's/^hw.//; s|/readme||i; s/^/readme./; s|/|-|g;')
    cp -a "$f" "$RELDIR/$d"
done

# add the .hex files
if [ "." = "$FILTER" ]; then
    rename -f 's|hex/anduril.|'"$RELDIR/hex/$RELNAME"'.|;' hex/*.hex
else
    rename -f 's|hex/anduril.|'"$RELDIR/hex/$RELNAME"'.|;' hex/*"$FILTER"*.hex
fi

# make a .zip file
cd releases
mkdir -p zip
ZNAME="$RELNAME"
[[ "." != "$FILTER" ]] && ZNAME="$RELNAME"."$FILTER"
ZPATH=zip/"$ZNAME".zip
zip -q -r "$ZPATH" "$ZNAME"
cd ..
ls -l "releases/$ZPATH"

