#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -s \
 -boot order=d \
 -drive file=disk.img,format=raw,media=disk,if=ide \
 -drive file=kernel.iso,media=cdrom,readonly=on \
 -serial file:logs.txt \
 -D /tmp/qemu-debug-log \
 -d cpu_reset \
 -m 512M \
 -cpu host -enable-kvm \
 -audio driver=pa,model=sb16,id=pa,out.frequency=48000,out.channels=2,out.format=s32 \
#-audio driver=pa,model=pcspk,id=pa \

