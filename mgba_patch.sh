#!/bin/sh -e

CURRENTPATH=`pwd`
cat $CURRENTPATH/formatting.c.add >> $CURRENTPATH/src/util/formatting.c
cd $CURRENTPATH/src/util
sed ':a;N;$!ba;s/return sceKernelLibcLocaltime_r(t, date);/return sceKernelLibcLocaltime_r(t, date);\n#elif defined(__CELLOS_LV2__)\n      *date = *localtime(t);\n      return date;/' formatting.c > formatting.c.mod
cat formatting.c.mod > formatting.c
cat $CURRENTPATH/common.h.add >> $CURRENTPATH/include/mgba-util/common.h
cd $CURRENTPATH/include/mgba-util
sed ':a;N;$!ba;s/#if !defined(PS2)/#if !defined(__CELLOS_LV2__)/' common.h > common.h.mod
cat common.h.mod > common.h
cd $CURRENTPATH/src/core
sed 's/#include <signal.h>//' thread.c > thread.c.mod
cat thread.c.mod > thread.c
cd $CURRENTPATH/src/platform/libretro
sed 's/gba|gb|gbc|sgb/gba|gb|gbc|sgb|u1|bin/' libretro.c > libretro.c.mod
cat libretro.c.mod > libretro.c
echo Patch done!