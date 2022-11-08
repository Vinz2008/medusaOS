#!/bin/sh
set -e
. ./iso.sh

bochs -f bochsrc.txt -q
