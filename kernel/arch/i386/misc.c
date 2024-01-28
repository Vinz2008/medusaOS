#include <kernel/io.h>
#include <kernel/misc.h>
#include <kernel/x86.h>

void reboot() {
  uint8_t good = 0x02;
  while (good & 0x02) {
    good = inb(0x64);
  }
  outb(0x64, 0xFE);
  x86_halt();
}