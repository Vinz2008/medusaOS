#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -s \
 -boot order=d \
 -drive file=disk.img,format=raw,media=disk,if=ide \
 -drive file=kernel.iso,media=cdrom,readonly=on \
 -device sb16 -audiodev driver=pa,id=pa,out.frequency=48000,out.channels=2,out.format=s32 \
 -serial file:logs.txt

