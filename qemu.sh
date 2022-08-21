#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) \
 -boot order=d \
 -drive file=disk.img,format=raw,media=disk,if=ide \
 -drive file=kernel.iso,media=cdrom,readonly=on \
 -serial file:logs.txt

