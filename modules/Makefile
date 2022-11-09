DEFAULT_HOST!=../default-host.sh
HOST?=DEFAULT_HOST
HOSTARCH!=../target-triplet-to-arch.sh $(HOST)

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Iinclude
LDFLAGS:=$(LDFLAGS)
#LIBS:=$(LIBS) -nostdlib -lk -lgcc

LDFLAGS:=$(LDFLAGS) -Tmod.ld

OBJS=\
start.o \
test_module.o \

.PHONY: all clean install
.SUFFIXES: .o .c .S

all: test_module.bin

LINK_LIST=\
$(LDFLAGS) \
$(LIBS) \

test_module.bin: $(OBJS) 
	$(LD) $(LDFLAGS) $(OBJS) -o $@
# objcopy -O binary -j .text $(OBJS) $@


.S.o: 
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
# objcopy -O binary -j .text test.o binfile

clean:
	rm -f test_module.bin 
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d


install: test_module.bin

-include $(OBJS:.o=.d)