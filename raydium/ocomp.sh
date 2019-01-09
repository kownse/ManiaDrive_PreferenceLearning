#!/bin/sh

# Small and ugly "static compiler" for Raydium apps, with ODE
# Use "./ocomp.sh test6.c" for example.

ulimit -c 0
rm test
gcc -g "$1" -Wall -O2 -o test -L/usr/X11R6/lib/ -lXinerama -lGL -lGLU -lm -lopenal -lalut -ljpeg \
-Iraydium/ode/include/ raydium/ode/ode/src/libode.a -lvorbis -lvorbisfile -logg \
-Iraydium/php/ -Iraydium/php/include -Iraydium/php/main/ -Iraydium/php/Zend -Iraydium/php/TSRM raydium/php/libs/libphp5.a \
-lresolv -lcrypt -lz -lcurl -lxml2 -lGLEW $OCOMP_FLAGS
sync
shift
./test "$@"
