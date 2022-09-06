#!/bin/sh
set -e
. ./build-x86_64.sh

rm -rf iso-x86_64
rm -rf sysroot-x86_64
rm -rf isodir
rm -rf iso-x86_64
mkdir sysroot-x86_64


mkdir sysroot-x86_64/BOOT
mkdir sysroot-x86_64/BOOT/grub
cp kernel/arch/x86_64/kernel.efi sysroot-x86_64/BOOT/kernel.efi

grub-mkstandalone -O x86_64-efi -o BOOTX64.EFI "BOOT/grub/grub.cfg=grub/build-x86_64/grub.cfg"
cp -v BOOTX64.EFI sysroot-x86_64/BOOT/BOOTX64.EFI
cp -v grub/grub-x86_64.cfg sysroot-x86_64/BOOT/grub/grub.cfg


dd if=/dev/zero of=medusa_os-efi.img  bs=512 count=93750
parted medusa_os-efi.img -s -a minimal mklabel gpt
parted medusa_os-efi.img -s -a minimal mkpart EFI FAT16 2048s 93716s
parted medusa_os-efi.img -s -a minimal toggle 1 boot

dd if=/dev/zero of=part-efi.img bs=512 count=91669
mformat -i part-efi.img -h 32 -t 32 -n 64 -c 1
mmd -i part-efi.img ::/EFI
mmd -i part-efi.img ::/EFI/BOOT
mcopy -v -i part-efi.img sysroot-x86_64/* ::
mcopy -v -i part-efi.img sysroot-x86_64/BOOT/BOOTX64.EFI ::/EFI/BOOT
dd if=part-efi.img of=medusa_os-efi.img bs=512 count=91669 seek=2048 conv=notrunc
mkdir iso-x86_64
cp medusa_os-efi.img iso-x86_64
xorriso -as mkisofs -R -f -e medusa_os-efi.img -no-emul-boot -o medusa_os-efi.iso iso-x86_64
