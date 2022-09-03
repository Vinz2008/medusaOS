#!/bin/sh
set -e
. ./build-x86_64.sh

rm -rf sysroot-x86_64
rm -rf isodir
mkdir sysroot-x86_64
mkdir sysroot-x86_64/boot
mkdir sysroot-x86_64/boot/grub
cp kernel/arch/x86_64/kernel.efi sysroot-x86_64/boot/kernel.efi

grub-mkstandalone -O x86_64-efi -o BOOTX64.EFI "boot/grub/grub.cfg=grub/build-x86_64/grub.cfg"
cp -v BOOTX64.EFI sysroot-x86_64/boot/BOOTX64.EFI
cp -v grub/grub-x86_64.cfg sysroot-x86_64/boot/grub/grub.cfg


dd if=/dev/zero of=medusa_os-efi.img  bs=512 count=93750
parted medusa_os-efi.img -s -a minimal mklabel gpt
parted medusa_os-efi.img -s -a minimal mkpart EFI FAT16 2048s 93716s
parted medusa_os-efi.img -s -a minimal toggle 1 boot

dd if=/dev/zero of=part-efi.img bs=512 count=91669
mformat -i part-efi.img -h 32 -t 32 -n 64 -c 1
mcopy -i part-efi.img sysroot-x86_64/* ::
dd if=part-efi.img of=medusa_os-efi.img bs=512 count=91669 seek=2048 conv=notrunc

