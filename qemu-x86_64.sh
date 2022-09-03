#!/bin/sh
set -e
. ./iso-x86_64.sh

qemu-system-x86_64 \
 -cpu qemu64 \
 -bios /usr/share/ovmf/x64/OVMF.fd \
 -drive file=medusa_os-efi.img,if=ide \
 -serial file:logs.txt