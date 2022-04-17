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
grub-mkrescue /usr/lib/grub/i386-pc -o kernel.iso isodir
