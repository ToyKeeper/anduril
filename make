#!/usr/bin/env bash
# Anduril / FSM "make" script
# Copyright (C) 2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later
#
# replaces system "make" command,
# because this project doesn't need make's features;
# it just needs a little wrapper script for common tasks

# enable "**" for recursive glob (requires bash)
shopt -s globstar

if [ "${DEBUG}" == "1" ]; then
  set -x
fi

# figure out which operation was requested
MODE="$1"

function help() {
  cat << ENDOFHELP
Anduril make: a build helper tool for Anduril flashlight firmware
Usage: ./make TASK
... where TASK is:

  help                 Show this help text
  (nothing)            Compile all build targets
  flash FILE           Flash firmare FILE to a hardware device
  clean                Delete generated files
  dfp                  Download and install Atmel DFPs
  docs                 Convert all .md files to .html
  models               Generate the MODELS file
  release              Zip up all .hex files to prep for publishing a release
  todo                 Show tasks noted in source code files
  docker-build TASK    Run TASK in the Docker builder (needs working Docker)

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
  # build d4k-3ch firmware using the Docker builder
  ./make docker-build d4k-3ch
ENDOFHELP
}

# sub-command parser / dispatcher
function main() {
  case "$MODE" in
    -h|--help|help|/\?|/h|/help)
      help
      ;;
    clean)
      echo 'rm -vf -- **/*~ hex/*.hex ui/**/*.elf ui/**/*.o ui/**/*.cpp'
      rm -vf -- **/*~ hex/*.hex ui/**/*.elf ui/**/*.o ui/**/*.cpp
      echo 'git checkout -- ui/anduril/version.h'
      git checkout -- ui/anduril/version.h
      ;;
    dfp)
      shift
      exec ./bin/dfp-install.sh "$@"
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
      grep -E 'TODO:|FIXME:' -- **/*.[ch] **/*.md
      ;;
    docker-build)
      shift
      # Cygwin is its own special case; Linux/Mac/WSL can be handled the same way
      if [[ $(uname -a | grep -i cygwin >/dev/null) ]]
      then
        SCRIPTPATH_CYGWIN="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
        if [[ "${SCRIPTPATH_CYGWIN}" =~ "^/cygdrive" ]]; then
          SRC_PATH="$(sed 's|cygdrive||' <<< \"${SCRIPTPATH_CYGWIN}\")" # e.g. /cygdrive/c/users/foo -> //c/users/foo
        else
          SRC_PATH="$(cygpath -m -w /)/${SCRIPTPATH_CYGWIN}"
        fi
      else
        SRC_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )" # Handles various edge cases better than a naive ${PWD} or $(pwd)
      fi
      docker run --pull=true --rm -v ${SRC_PATH}:/src -it siterelenby/anduril-builder:latest ${@}
      ;;
    *)
      exec ./bin/build-all.sh "$@"
      ;;
  esac
}

function make-docs () {
  for md in **/*.md ; do
    html=${md//.md/.html}
    echo "$md --> $html"
    cmark-gfm "$md" > "$html"
  done
}

# go to the repo root
BASEDIR=$(dirname "$0")
cd "$BASEDIR" || (echo "Error: Can't cd to basedir." && exit 1)

# do whatever the user requested
main "$@"

