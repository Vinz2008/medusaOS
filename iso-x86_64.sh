#!/bin/sh
set -e
. ./build-x86_64.sh

rm -rf iso-x86_64
rm -rf sysroot-x86_64
rm -rf isodir
rm -rf iso-x86_64
mkdir sysroot-x86_64
mkdir sysroot-x86_64/EFI


mkdir sysroot-x86_64/EFI/Boot
mkdir sysroot-x86_64/EFI/Boot/grub
cp kernel/arch/x86_64/kernel.efi sysroot-x86_64/EFI/Boot/kernel.efi

grub-mkstandalone -O x86_64-efi -o BOOTX64.EFI "EFI/Boot/grub/grub.cfg=grub/build-x86_64/grub.cfg"
cp -v BOOTX64.EFI sysroot-x86_64/EFI/Boot/BOOTX64.EFI
cp -v grub/grub-x86_64.cfg sysroot-x86_64/EFI/Boot/grub/grub.cfg


dd if=/dev/zero of=medusa_os-efi.img  bs=512 count=93750
parted medusa_os-efi.img -s -a minimal mklabel gpt
parted medusa_os-efi.img -s -a minimal mkpart EFI FAT16 2048s 93716s
parted medusa_os-efi.img -s -a minimal toggle 1 boot

dd if=/dev/zero of=part-efi.img bs=512 count=91669
mformat -i part-efi.img -h 32 -t 32 -n 64 -c 1
mcopy -i part-efi.img sysroot-x86_64/* ::
dd if=part-efi.img of=medusa_os-efi.img bs=512 count=91669 seek=2048 conv=notrunc
mkdir iso-x86_64
cp medusa_os-efi.img iso-x86_64
xorriso -as mkisofs -R -f -e medusa_os-efi.img -no-emul-boot -o medusa_os-efi.iso iso-x86_64
