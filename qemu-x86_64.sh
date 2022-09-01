#!/bin/sh
set -e
. ./iso-x86_64.sh

qemu-system-x86_64 \
 -bios /usr/share/ovmf/x64/OVMF.fd
 -serial file:logs.txt