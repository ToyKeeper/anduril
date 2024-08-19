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

# releases are named "$project.$revision"
RELNAME="anduril.$REV"

# release directory
RELDIR="releases/$RELNAME"
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

# add hardware-specific docs
for f in $(find hw -name '*.md') ; do
    d=$(echo "$f" | sed 's/^hw.//; s|/readme||i; s/^/readme./; s|/|-|g;')
    cp -a "$f" "$RELDIR/$d"
done

# add the .hex files
rename -f 's|hex/anduril.|'"$RELDIR/hex/$RELNAME"'.|;' hex/*.hex

# make a .zip file
cd releases
mkdir -p zip
ZPATH=zip/"$RELNAME".zip
zip -q -r "$ZPATH" "$RELNAME"
cd ..
ls -l "releases/$ZPATH"

