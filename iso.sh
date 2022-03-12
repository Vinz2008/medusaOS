#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/kernel.kernel isodir/boot/kernel.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "kernel" {
	multiboot /boot/kernel.kernel
}
EOF
grub-mkrescue /usr/lib/grub/i386-pc -o kernel.iso isodir
