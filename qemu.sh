#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -s -M q35 \
 -boot order=d \
 -drive file=disk.img,id=disk,format=raw,media=disk,if=none \
 -device ahci,id=ahci \
 -device ide-hd,drive=disk,bus=ahci.0 \
 -drive file=kernel.iso,media=cdrom,readonly=on \
 -serial file:logs.txt \
 -D /tmp/qemu-debug-log \
 -d cpu_reset \
 -m 512M \
 -cpu host -enable-kvm \
 -audio driver=pa,model=sb16,id=pa,out.frequency=48000,out.channels=2,out.format=s32 \
#-audio driver=pa,model=pcspk,id=pa \
#-audiodev pa,id=audio0 -machine pcspk-audiodev=audio0 \
