#!/bin/sh

UI=anduril

for TARGET in \
  BLF_GT \
  BLF_GT_MINI \
  BLF_LANTERN \
  BLF_Q8 \
  EMISAR_D1 \
  EMISAR_D1S \
  EMISAR_D4 \
  EMISAR_D4_219C \
  EMISAR_D4S \
  EMISAR_D4S_219C \
  FF_PL47 \
  FF_ROT66 \
  FF_ROT66_219 \
  FW3A \
  SOFIRN_SP36 \
  ; do
  echo "===== $TARGET ====="
  ../../../bin/build.sh 85 "$UI" "-DFSM_${TARGET}_DRIVER"
  mv -f "$UI".hex "$UI".$TARGET.hex
done
