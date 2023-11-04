#!/bin/bash
# Anduril / FSM "make" script
# Copyright (C) 2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later
#
# replaces system "make" command,
# because this project doesn't need make's features;
# it just needs a little wrapper script for common tasks

# enable "**" for recursive glob (requires bash)
shopt -s globstar

# figure out which operation was requested
MODE="$1"

function help() {
  cat << ENDOFHELP
Anduril make: a build helper tool for Anduril flashlight firmware
Usage: ./make TASK
... where TASK is:

  help            Show this help text
  (nothing)       Compile all build targets
  flash FILE      Flash firmare FILE to a hardware device
  clean           Delete generated files
  dfp             Download and install Atmel DFPs
  docs            Convert all .md files to .html
  models          Generate the MODELS file
  release         Zip up all .hex files to prep for publishing a release
  todo            Show tasks noted in source code files

... or TASK can be the partial name of a build target.

Examples:

  # get rid of old clutter files
  ./make clean
  # compile all anduril build targets
  ./make anduril
  # compile all builds matching "emisar" AND "nofet"
  ./make emisar nofet
  # compile all builds matching "q8" (i.e. Sofirn BLF Q8)
  ./make q8
  # Flash the Q8 firmware built in the previous command
  # (copy/paste the file path printed by the build script)
  ./make flash hex/sofirn-blf-q8.hex
ENDOFHELP
}

# sub-command parser / dispatcher
function main() {
  case "$MODE" in
    -h|--help|help|/?|/h|/help)
      help
      ;;
    clean)
      echo 'rm -vf **/*.hex **/*~ **/*.elf **/*.o **/*.cpp'
      rm -vf **/*.hex **/*~ **/*.elf **/*.o **/*.cpp
      ;;
    dfp)
      shift
      ./bin/dfp-install.sh "$@"
      ;;
    docs)
      make-docs
      ;;
    flash)
      echo "Not implemented yet."
      #./bin/flash.sh "$@"
      ;;
    models)
      ./bin/models.py > MODELS
      cat MODELS
      ;;
    release)
      echo "Not implemented yet."
      ;;
    todo)
      grep -E 'TODO:|FIXME:' **/*.[ch]
      ;;
    *)
      ./bin/build-all.sh "$@"
      ;;
  esac
}

function make-docs () {
  for md in **/*.md ; do
    echo "$md"
    html=$(echo "$md" | sed 's/.md$/.html/')
    cmark-gfm "$md" > "$html"
  done
}

# go to the repo root
BASEDIR=$(dirname $0)
cd "$BASEDIR"

# do whatever the user requested
main "$@"

