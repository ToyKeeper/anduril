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

# Default to build date until support for hex in version numbers exists
export DEFAULT_VERSION_NUMBER_SCHEME=build-date
#DEFAULT_VERSION_NUMBER_SCHEME=official-release

# This should match the release pattern(s) used on the main repo
export DEFAULT_RELEASE_PATTERN="r*"

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

Command-line arguments:
  --help -h help /? /h /help                       Show this message
  --debug                                          Show debug output from scripts
  --version-string <string>                        Use <string> for the version number. Acceptable characters are 0-9 and '-'.
  --version-build-date                             Use the date the firmware was built as a version number
  --version-number-scheme <scheme>                 Use the stated version number scheme. Options are:
                                                   - official-release: base the version number on the newest release from the project repo (default)
                                                   - local-tags: base the version number on tags present within the local repo
                                                   - local-releases: base the version number on tags present within the local repo that match the defined pattern to be considered releases
                                                   - build-date: use the date that the firmware was built on. This was the default before 2023-10-31
  --release-patterns <patterns>                    List of glob patterns for which tags should be considered releases, separated by ';', e.g. "r*;dev*" (default "r*")
  --fetch-tags-fail-action [abort|continue|fallback]    If fetching tags from a remote repository, whether the build should abort, continue with cached tags, or fallback to a local build date

By default the newest tag relevant to the working state of the source will be used to generate a version number, and if no tags are found, the user will be asked what to do, or the build fail if automated.

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
    clean)
      echo 'rm -vf -- **/*~ hex/*.hex ui/**/*.elf ui/**/*.o ui/**/*.cpp'
      rm -vf -- **/*~ hex/*.hex ui/**/*.elf ui/**/*.o ui/**/*.cpp
      echo 'git checkout -- ui/*/version.h'
      git checkout -- ui/*/version.h
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

# Parse and validate command line args
POSITIONAL=()
while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
    --debug)
      export DEBUG=1
      shift
    ;;
    --help|help|-h|/\?|/h|/help)
      help
      exit 255
    ;;
    --fetch-tags-fail-action*)
      shift # past arg
      if [[ "${key}" =~ ^\-\-fetch\-tags\-fail\-action= ]] #is this in the form of --foo=bar?
      then
        FETCH_TAGS_FAIL_ACTION="$(cut -d '=' -f2 <<< "${key}")"
      elif [[ -z "${1}" ]] || [[ "${1}" =~ ^\-\- ]] # is this followed by another long arg, or by nothing?
      then
        echo "--fetch-tags-fail-action requires an argument"
        exit 1
      else # in the form of --foo bar
        FETCH_TAGS_FAIL_ACTION="${1}"
        shift # past value
      fi
      case ${FETCH_TAGS_FAIL_ACTION} in
        abort|continue|fallback)
          export FETCH_TAGS_FAIL_ACTION
        ;;
        *)
          echo "ERROR: unknown --fetch-tags-fail-action argument ${FETCH_TAGS_FAIL_ACTION}" >&2
          exit 1
        ;;
      esac
    ;;
    --version-build-date)
      shift # past arg
      export VERSION_NUMBER_SCHEME="build-date"
    ;;
    --version-string*)
      shift # past arg
      export VERSION_NUMBER_SCHEME="manual"
      if [[ "${key}" =~ ^\-\-version\-string= ]] #is this in the form of --foo=bar?
      then
        VERSION_STRING="$(cut -d '=' -f2 <<< "${key}")"
      elif [[ -z "${1}" ]] || [[ "${1}" =~ ^\-\- ]] # is this followed by another long arg, or by nothing?
      then
        echo "--version-string requires an argument"
        exit 1
      else # in the form of --foo bar
        VERSION_STRING="${1}"
        shift # past value
      fi
      if [[ ! "${VERSION_STRING}" =~ ^[0-f][\-0-f]*[0-f]$ ]]
      then
        echo "ERROR: version number string can only contain the characters '-' and 0-f and can not start or end with '-'" >&2
        exit 1
      else
        export VERSION_STRING
      fi
      export VERSION_STRING
    ;;
    --version-number-scheme*)
      shift # past arg
      if [[ "${key}" =~ ^\-\-version\-number\-scheme= ]] #is this in the form of --foo=bar?
      then
        VERSION_NUMBER_SCHEME="$(cut -d '=' -f2 <<< "${key}")"
      elif [[ -z "${1}" ]] || [[ "${1}" =~ ^\-\- ]] # is this followed by another long arg, or by nothing?
      then
        echo "--version-number-scheme requires an argument"
        exit 1
      else # in the form of --foo bar
        VERSION_NUMBER_SCHEME=${1}
        shift # past value
      fi
      case ${VERSION_NUMBER_SCHEME} in
      build-date|local-releases|local-tags|official-release)
        export VERSION_NUMBER_SCHEME
        ;;
      *)
        echo "ERROR: unknown --version-number-scheme argument ${VERSION_NUMBER_SCHEME}" >&2
        exit 1
      ;;
      esac
    ;;
    --release-patterns*)
      shift # past arg
      if [[ "${key}" =~ ^\-\-release\-patterns= ]] #is this in the form of --foo=bar?
      then
        RELEASE_PATTERNS="$(cut -d '=' -f2 <<< "${key}")"
      elif [[ -z "${1}" ]] || [[ "${1}" =~ ^\-\- ]] # is this followed by another long arg, or by nothing?
      then
        echo "--release-patterns requires an argument"
        exit 1
      else # in the form of --foo bar
        RELEASE_PATTERNS=${1}
        shift # past value
      fi
      export RELEASE_PATTERNS
    ;;
    *)
      POSITIONAL+=("$1") # save it in an array for later
      shift # past argument
    ;;
  esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

[[ -z "${VERSION_NUMBER_SCHEME}" ]] && export VERSION_NUMBER_SCHEME=${DEFAULT_VERSION_NUMBER_SCHEME}

# go to the repo root
BASEDIR=$(dirname "$0")
cd "$BASEDIR" || (echo "Error: Can't cd to basedir." && exit 1)

# do whatever the user requested
main "$@"

