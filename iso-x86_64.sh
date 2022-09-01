#!/bin/sh
set -e
. ./build-x86_64.sh

rm -rf sysroot-x86_64
rm -rf isodir
mkdir sysroot-x86_64
mkdir sysroot-x86_64/boot
mkdir sysroot-x86_64/boot/grub
cp kernel/arch/x86_64/kernel.efi sysroot-x86_64/boot/kernel.efi

grub-mkstandalone -O x86_64-efi -o BOOTX64.EFI "boot/grub/grub.cfg=grub/grub-x86_64-build.cfg"
cp -v BOOTX64.EFI sysroot-x86_64/BOOTX64.EFI

dd if=/dev/zero of=medusa_os-efi.img bs=1048576 count=128
echo -e "g\nn\n1\n2048\n+8M\nt 1\n1\nw" | fdisk medusa_os-efi.img

