#!/bin/sh
set -e
. ./config.sh

for PROJECT in $PROJECTS; do
  (cd $PROJECT && $MAKE clean)
done

cd modules && $MAKE clean && cd ..
cd userspace && $MAKE clean && cd ..

rm -rf isodir
rm -rf kernel.iso
rm -f disk.img
rm -f *.img
rm -f *.iso
rm -f logs.txt
#rm -rf memtest86plus
rm -rf sysroot
rm -rf sysroot-x86_64
rm -rf build
rm -f bochslog.txt
rm -f *.efi
rm -f *.EFI
rm -rf iso-x86_64