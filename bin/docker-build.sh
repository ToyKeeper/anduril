#! /usr/bin/env bash

# Build anduril using the Docker builder (https://github.com/SiteRelEnby/anduril-buildenv-docker)
# All arguments to this script are passed to the Docker container, which should autodetect the repo format and call the make script with them
# Compatible with Linux, Windows (Cygwin and WSL) and MacOS, as long as the user has working Docker. Currently supports amd64, arm64, and armv7.

if [ "${DEBUG}" == "1" ]; then
  set -x
fi

set -e -o pipefail
# Cygwin is its own special case; Linux/Mac/WSL can be handled the same way
# shellcheck disable=SC2143
if (uname -a | grep -i -q cygwin)
then
  SCRIPTPATH_CYGWIN="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 || exit ; cd .. && pwd -P )" # Handles various edge cases better than a naive ${PWD} or $(pwd)
  if [[ "${SCRIPTPATH_CYGWIN}" =~ "^/cygdrive" ]]; then
    # shellcheck disable=SC2001
    SRC_PATH="$(sed 's|cygdrive||' <<< \""${SCRIPTPATH_CYGWIN}\"")" # e.g. /cygdrive/c/users/foo -> //c/users/foo
  else
    SRC_PATH="$(cygpath -m -w /)/${SCRIPTPATH_CYGWIN}" # e.g. /home/foo/ -> C:/cygwin64/home/foo
  fi
else
  # Linux/WSL/Mac
  SRC_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 || exit ; cd .. && pwd -P )" # Handles various edge cases better than a naive ${PWD} or $(pwd)
fi
docker run --pull=always --rm -v "${SRC_PATH}":/src -it siterelenby/anduril-builder:latest "${@}"
