#!/bin/sh
set -e

if [ -z "$1" ]
  then
    . ./config.sh
  else 
    if [ "$1" = "x86_64" ]
    then
      . ./config.sh x86_64
    else 
      . ./config.sh
    fi
fi


mkdir -p "$SYSROOT"

for PROJECT in $SYSTEM_HEADER_PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install-headers)
done
