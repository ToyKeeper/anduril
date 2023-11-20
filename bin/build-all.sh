#!/usr/bin/env bash
# Anduril / FSM build wrapper script
# Copyright (C) 2017-2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

# Usage: build-all.sh [pattern]
# If pattern given, only build targets which match.

# enable "**" for recursive glob (requires bash)
shopt -s globstar

if [ "${DEBUG}" == "1" ]; then
  set -x
  export DEBUG
fi

if [ "$#" -gt 0 ]; then
  # multiple search terms with "AND"
  SEARCH=( "$@" )
  # memes
  [ "$1" = "me" ] && shift && shift && echo "Make your own $*." && exit 1
fi

# TODO: detect UI from $0 and/or $*
export UI=anduril

mkdir -p hex

# TODO: use a git tag for the version, instead of build date
# TODO: use build/version.h instead of $UI/version.h ?

case "${VERSION_NUMBER_SCHEME}" in
  ""|"builddate"|"build-date")
  #date '+#define VERSION_NUMBER "%Y-%m-%d"' > ui/$UI/version.h
  VERSION_STRING=$(date '+%Y-%m-%d')
  ;;
  "manual")
    #$VERSION_STRING should have already been set
    if [[ -z "${VERSION_STRING}" ]]
    then
      echo "ERROR: version number scheme set to manual but VERSION_STRING not found" >&2
      exit 3
    fi
  ;;
  "official-release")
    USE_CALC_VERSION=1
    TAG_UPSTREAM="git+ssh://git@github.com/toykeeper/anduril"
    TAG_UPSTREAM_FALLBACK="https://github.com/toykeeper/anduril"
    # TODO: pull list of releases from github when it's done and build from that instead?
    RELEASE_PATTERNS="r*"
  ;;
  "local-tags")
    USE_CALC_VERSION=1
    TAG_UPSTREAM=""
    RELEASE_PATTERNS=""
  ;;
  "local-releases")
    USE_CALC_VERSION=1
    TAG_UPSTREAM=""
    TAG_UPSTREAM_FALLBACK=""
    if [[ -z "${RELEASE_PATTERNS}" ]]
    then
      echo "No release patterns found; using default (${DEFAULT_RELEASE_PATTERN}). To override, use --release-patterns or env var RELEASE_PATTERNS with a list of glob patterns to consider as releases" >&2
      RELEASE_PATTERNS=${DEFAULT_RELEASE_PATTERN}
    fi
  ;;
esac


fetch_tags_fail(){
  case $FETCH_TAGS_FAIL_ACTION in
    abort)
      echo "ERROR: failed to fetch tags" >&2
      exit 1
    ;;
    continue)
      echo "WARNING: failed to fetch tags" >&2
    ;;
    fallback)
      echo "WARNING: failed to fetch tags; falling back to build date for version number" >&2
      VERSION_STRING=$(date '+%Y-%m-%d')
      SKIP_CALC_VERSION=1
    ;;
    *)
      echo "Failed to fetch tags. To suppress this message, you can use --fetch-tags-fail-action=[abort|continue]" >&2
      exit 1
    ;;
  esac
}

if [[ "${USE_CALC_VERSION}" == "1" ]]
then
  if [[ -n "${RELEASE_PATTERNS}" ]]
  then
    release_pattern_refs=()
    release_pattern_match=()
    IFS=";" read -r -a patterns <<< "${RELEASE_PATTERNS}"
    for pattern in "${patterns[@]}"
    do
      #TODO: build *specific* release pattern match based on remote release list?
      release_pattern_refs+=("refs/tags/${pattern}")
      release_pattern_match+=("--match ${pattern}")
    done
  fi

  if [[ -n "${TAG_UPSTREAM}" ]]
  then
    git fetch --tags "${TAG_UPSTREAM}"
    # shellcheck disable=SC2181
    if [[ "${?}" != "0" ]]
    then
      if [[ -n "${TAG_UPSTREAM_FALLBACK}" ]]
      then
        git fetch --tags "${TAG_UPSTREAM_FALLBACK}"
        if [[ "${?}" != "0" ]]
        then
          fetch_tags_fail
        fi
      else
        fetch_tags_fail
      fi
    fi
  fi

  if [[ -z "${SKIP_CALC_VERSION}" ]]
  then
    #shellcheck disable=SC2068
    DIRTY=$(git describe --tags --dirty ${release_pattern_match[@]} )

    if grep -qE "\-dirty$" <<< "${DIRTY}"
    then
      IS_DIRTY="-1"
      # do a shift for next part
      DIRTY_TMP="${DIRTY}"
      # shellcheck disable=SC2001
      DIRTY=$(sed 's/-dirty$//' <<< "${DIRTY_TMP}")
    else
      IS_DIRTY=""
    fi

    #if last part is g followed by some hex with at least one part before it, it's a commit hash of where we are
    if grep -qE ".*-g[0-f]+$" <<< "${DIRTY}"
    then
      # TODO is there 100% always a commithash if there is a number of commits since tag? would be a weird edge case if not
      #COMMITHASH_AFTER_TAG=$(sed -r -n 's/^.*-(g[0-f]+)/\1/p' <<< "${DIRTY}")
      COMMITHASH_AFTER_TAG=$(sed -r -n 's/^.*-g([0-f]+)/\1/p' <<< "${DIRTY}")
      echo "COMMITHASH_AFTER_TAG=${COMMITHASH_AFTER_TAG}"

      #do a shift for next part
      DIRTY_TMP="${DIRTY}"
      DIRTY=$(sed -r 's/-g[0-f]+$//' <<< "${DIRTY_TMP}")
      #if last part is an integer, with at least one more part before it, it's commits since tag
      if grep -qE ".*-[0-9]+$" <<< "${DIRTY}"
      then
        COMMITS_SINCE_TAG=$(grep -oE "[0-9]+$" <<< "${DIRTY}")
        echo COMMITS_SINCE_TAG="${COMMITS_SINCE_TAG}"

        #do a shift for next part
        DIRTY_TMP="${DIRTY}"
        DIRTY=$(sed -r 's/-[0-9]+$//' <<< "${DIRTY_TMP}")
      fi
    else
      COMMITS_SINCE_TAG=0
      COMMITHASH_AFTER_TAG=""
    fi

    # rest should be the tag we have the status for
    COMMITHASH_BASE="${DIRTY}"
    echo "COMMITHASH_BASE=${COMMITHASH_BASE}"

    # TODO: verify against remote repo here?

    TAG_DATE=$(git log -1 --format=%cs r2023-10-31)

    VERSION_STRING="${TAG_DATE}-${COMMITS_SINCE_TAG}${IS_DIRTY}-${COMMITHASH_AFTER_TAG}"
  fi
fi
echo "#define VERSION_NUMBER \"${VERSION_STRING}\"" > ui/$UI/version.h

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
  echo "===== $UI : $NAME ====="

  # try to compile, track result, and rename compiled files
  if bin/build.sh "$TARGET" ; then
    mv -f "ui/$UI/$UI".hex "hex/$UI.$NAME.hex"
    echo "  > hex/$UI.$NAME.hex"
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
