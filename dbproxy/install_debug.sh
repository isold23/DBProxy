#!/bin/sh
killall -9 dbproxy
killall -9 dbproxy_test
autoheader
touch NEWS README AUTHORS ChangeLog
touch stamp-h
aclocal
libtoolize -c -f
autoconf
automake --add-missing
./configure --enable-debug --prefix=$(pwd)
make uninstall;make clean;make all;make install
