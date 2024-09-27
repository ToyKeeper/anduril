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

# trap ^C to show the build summary on aborted builds, if we got that far
function ctrlc(){
    # output the build summary if the build was cancelled but a build was attempted beforehand
    if [ "$PASS" -gt 0 ] || [ "$FAIL" -gt 0 ]
    then
        echo #push a new line below the ^C char for neatness
        build_summary
        exit 1
    else
        exit 1
    fi
}

trap ctrlc INT

function build_help(){
    cat << ENDOFHELP

Advanced build options:
  --build-help    Show this help text
  --user USER     Load user's custom configuration from users/USER
  --no-user       Do not load any custom user configuration; build default targets only
  --debug         Enable script debug output
  --save-cpp      Save the intermediate .cpp source file (usually for debugging preprocessor macros)
  --show-passed   Output a list of successful builds, in addition to any failed

ENDOFHELP
    exit 2
}

USER_CONFIGS=()
POSITIONAL=()
while [ "${#}" -gt "0" ]
do
    [ "${DEBUG}" == "1" ] && echo "parsing arg: ${1}"

    if [[ "${1}" =~ "--user" ]]
    then
        # load user config

        # using --foo=bar instead of --foo bar
        if [[ "${1}" =~ "--user=" ]]
        then
            #extract the actual arg part
            arg=$(cut -d= -f2- <<< "${1}")
        else
            shift # past arg
            # save the arg value
            arg="${1}"
        fi
        if [[ -n "${arg}" ]]
        then
            if [ -d "users/${arg}" ]
            then
                echo "Loading user config from 'users/${arg}'" >&2
                USER_CONFIGS+=("${1}") # store user in an array to support multiple users
            else
                echo "ERROR: directory 'users/${arg}' not found" >&2
                exit 1
            fi
        else
            # e.g. --user= or --user on the end of the cmdline with no arg
            echo "ERROR: --user needs an arg" >&2
            exit 1
        fi
    elif [[ "${1}" =~ --no-user ]]
    then
        # clear all user configs (may be re-instantiated later e.g. --no-user --user foo)
	unset USER_CONF USER_CFG_DIR USER_CFG_DIRNAME USER_CONFIGS
        export SKIP_USER_CFG_AUTOLOAD=1 # TODO: remove?
    elif [[ "${1}" == "--debug" ]]
    then
        set -x
        export DEBUG=1 # cascade to downstream scripts
    elif [[ "${1}" == "--save-cpp" ]]
    then
        # save the rendered .cpp, e.g. for preprocessor debugging
        export SAVE_CPP=1
    elif [[ "${1}" == "--show-passed" ]]
    then
        #enable output of list of passed builds
        export SHOW_PASSED=1
    elif [[ "${1}" == "--build-help" ]]
    then
        build_help
    else
        # Store var in a temporary array to restore after we've finished parsing for builder-specific args
        POSITIONAL+=("$1")
    fi
    shift # past arg or value
    [ "${DEBUG}" == "1" ] && echo "${@}"
done
set -- "${POSITIONAL[@]}" # restore positional parameters

function build {
	TARGET=${1}

        # announce what we're going to build
        echo "===== $UI $REV : ${NAME}${NAME_USERPART} ====="

        # if saving the .cpp (for preprocessor debugging, etc) then render the filename for that
        if [ "${SAVE_CPP}" == "1" ]
        then
            CPP_SAVE="${NAME}${NAME_USERPART}.cpp"
        else
            CPP_SAVE=""
        fi

        # try to compile, track result, and rename compiled files
        if USER_DEFAULT_CFG="${USER_DEFAULT_CFG}" USER_MODEL_CFG="${USER_MODEL_CFG}" CPP_SAVE="${CPP_SAVE}" bin/build.sh "$TARGET" ; then

            # if this is a build with user modifications, we need to name it as such. TODO: allow multiple model-specific custom builds
            if [ -z "${USER_DEFAULT_CFG}" ] && [ -z "${USER_MODEL_CFG}" ]
            then
                HEX_USERPART="" # noop
            else
                HEX_USERPART="_${USER_CFG}"
            fi

            HEX_OUT="hex/$UI.${NAME}${HEX_USERPART}.hex"
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
}

function search_build_targets {
    # build targets are hw/$vendor/$model/**/$ui.h
    for TARGET in hw/*/*/**/"$UI".h ; do

        # friendly name for this build
        NAME=$(perl -ne 's|/|-|g; /hw-(.*)-'"$UI"'.h/ && print "$1\n";' <<< "$TARGET")

        # limit builds to searched patterns, if given
        SKIP=0
        if [ ${#SEARCH[@]} -gt 0 ]; then
            for text in "${SEARCH[@]}" ; do
                if ! grep -i -- "$text" <<< "$NAME $TARGET" > /dev/null ; then
                    SKIP=1
                fi
            done
        fi
        if [ 1 = $SKIP ]; then continue ; fi

        if [ ${#USER_CONFIGS[@]} -ne 0 ]; then
            for USER_CFG in "${USER_CONFIGS[@]}"
            do
                [ -f "users/${USER_CFG}/${UI}.h" ] && export USER_DEFAULT_CFG="users/${USER_CFG}/${UI}.h"
                # TODO: allow multiple custom model builds per user
                [ -f "users/${USER_CFG}/models/${NAME}/${UI}.h" ] && export USER_MODEL_CFG="users/${USER_CFG}/models/${NAME}/${UI}.h"
                # TODO: multiple user cfgs. possible implementations:
                #    models/wurkkos-ts10/anduril_config1.h models/wurkkos-ts10/anduril_config2.h -> _user_config1 _user_config2 ?
                #    or models/wurkkos-ts10/anduril/config1/anduril.h models/wurkkos-ts10/anduril/config2/anduril.h -> _user_config1 _user_config2 ?
                #    second one looks nicer, I guess... either way, for now we only need the name part until this is implemented
                NAME_USERPART="_${USER_CFG}"

                if [ -z "${USER_DEFAULT_CFG}" ] && [ -z "${USER_MODEL_CFG}" ]
                then
                    echo -e "=====\nskipping $TARGET (no matching custom config for user ${USER_CFG})"
                else
                    build "${TARGET}"
                fi
                unset USER_DEFAULT_CFG USER_MODEL_CFG
            done
            if [ -z "${SKIP_DEFAULT_BUILDS}" ]
            then
                NAME_USERPART=""
                build "${TARGET}"
            fi
        else
            NAME_USERPART=""
            build "${TARGET}"
	fi
    done
}

function main {
    if [ "$#" -gt 0 ]; then
        # multiple search terms with "AND"
        SEARCH=( "$@" )
        # memes
        [ "$1" = "me" ] && shift && shift && echo "Make your own $*." && exit 1
    fi

    # Get a default user config to use, if there is one and it isn't already overridden
    if [ -f users.cfg ] && [ -z "${SKIP_USER_CFG_AUTOLOAD}" ] && [ "${#USER_CONFIGS[@]}" == 0 ]
    then
        echo "Reading custom users from users.cfg; to skip use --no-user" >&2
        while read -r line || [ -n "$line" ]
        do
            if [[ -d "users/${line}" ]]
            then
                USER_CONFIGS+=("${line}") # store user in an array to support multiple users
                echo "Loaded user config dir ${line} from users.cfg" >&2
            else
                echo "Warning: user config dir ${line} not found" >&2
            fi
        done < users.cfg
    fi

    # TODO: detect UI from $0 and/or $*
    UI=anduril

    [ ! -d hex/ ] && mkdir -p hex

    make-version-h  # generate a version.h file

    PASS=0
    FAIL=0
    PASSED=''
    FAILED=''

    search_build_targets

    build_summary
}

function build_summary(){
    # summary
    echo "===== $PASS builds succeeded, $FAIL failed ====="
    [ -n "${SHOW_PASSED}" ] && echo "PASS: $PASSED"
    if [ 0 != $FAIL ]; then
        echo "FAIL: $FAILED"
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

