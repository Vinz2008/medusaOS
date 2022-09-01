#!/bin/sh
set -e
. ./build-x86_64.sh


mkdir sysroot-x86_64
cp kernel/arch/x86_64/kernel.efi sysroot-x86_64/boot/kernel.efi
cp grub/grub.cfg isodir/boot/grub/grub.cfg

grub-mkstandalone -O x86_64-efi -o BOOTX64.EFI "boot/grub/grub.cfg=isodir/boot/grub/grub.cfg"