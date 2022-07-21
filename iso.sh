#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
mkdir -p isodir/modules

cp -r sysroot/* isodir/
#cp sysroot/boot/kernel.kernel isodir/boot/kernel.kernel
cp grub/* isodir/boot/grub/

if ![ -e x.memtest86plus ]
then
git clone https://github.com/memtest86plus/memtest86plus.git
fi
cd memtest86plus
cd build32
make
cd ../..
cp memtest86plus/build32/memtest.bin isodir/boot/memtest.bin


grub-mkrescue /usr/lib/grub/i386-pc -o kernel.iso isodir
