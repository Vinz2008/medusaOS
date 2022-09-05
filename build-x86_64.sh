#!/bin/sh
set -e


. ./headers.sh x86_64


for PROJECT in $PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE -f Makefile.x86_64 install)
done