#!/bin/sh

UI=anduril

for TARGET in \
  BLF_GT \
  BLF_GT_MINI \
  BLF_Q8 \
  EMISAR_D1 \
  EMISAR_D1S \
  EMISAR_D4 \
  EMISAR_D4S \
  EMISAR_D4S_219c \
  FF_ROT66 \
  FW3A \
  ; do
  echo "===== $TARGET ====="
  ../../../bin/build-85.sh "$UI" "-DFSM_${TARGET}_DRIVER"
  mv -f "$UI".hex "$UI".$TARGET.hex
done
