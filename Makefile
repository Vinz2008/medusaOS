all:
	./build.sh

iso:
	./iso.sh

qemu:
	./qemu.sh

onefetch:
	./onefetch.sh

clean:
	./clean.sh

format: FORMAT_FILES = $(shell find kernel -name '*.c' -o -name '*.h') $(shell find libc -name '*.c' -o -name '*.h') $(shell find modules -name '*.c' -o -name '*.h')
format:
	clang-format -i $(FORMAT_FILES)