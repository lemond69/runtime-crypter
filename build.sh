#!/bin/sh
i686-w64-mingw32-gcc main.c -finstrument-functions -c -o main.o -m32 -O1
i686-w64-mingw32-gcc -c -o hook.o hook.c -m32 -O2
i686-w64-mingw32-gcc main.o hook.o -m32 -s
