#!/bin/sh

UI=anduril

for TARGET in cfg-*.h ; do
  NAME=$(echo "$TARGET" | perl -ne '/cfg-(.*).h/ && print "$1\n";')
  echo "===== $NAME ====="
  echo ../../../bin/build.sh 85 "$UI" "-DCONFIGFILE=${TARGET}"
  ../../../bin/build.sh 85 "$UI" "-DCONFIGFILE=${TARGET}"
  mv -f "$UI".hex "$UI".$NAME.hex
done
