all:
	./build.sh

iso:
	./iso.sh

qemu:
	./qemu.sh

onefetch:
	./onefetch.sh



format: FORMAT_FILES = $(shell find kernel -name '*.c') $(shell find libc -name '*.c') $(shell find modules -name '*.c')
format:
	clang-format -i $(FORMAT_FILES)