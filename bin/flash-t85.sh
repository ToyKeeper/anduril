#!/bin/sh

# This script should be a symlink to flash-generic.sh,
# but the repo might be checked out on a filesystem with no symlinks.
# So... kludge together similar behavior.

BASEDIR=$(dirname -- "$0")
exec $BASEDIR/flash-generic.sh "$@" t85

