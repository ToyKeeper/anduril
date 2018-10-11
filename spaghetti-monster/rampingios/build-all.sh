#!/bin/sh

cp -av ../anduril/cfg*.h .

UI=rampingiosv3

for TARGET in \
  EMISAR_D1 \
  EMISAR_D1S \
  EMISAR_D4 \
  EMISAR_D4_219C \
  EMISAR_D4S \
  EMISAR_D4S_219C \
  ; do
  echo "===== $TARGET ====="
  ../../../bin/build-85.sh "$UI" "-DFSM_${TARGET}_DRIVER"
  mv -f "$UI".hex "$UI".$TARGET.hex
done
