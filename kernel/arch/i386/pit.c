#include <types.h>
#include <kernel/io.h>
#include <kernel/pit.h>

extern cli();

unsigned read_pit_count(void) {
	unsigned count = 0;
	cli();
	outb(0x43,0b0000000);
	count = inb(0x40);
	count |= inb(0x40) <<8;
	return count;
}

void set_pit_count(unsigned count) {
	cli();
	outb(0x40,count & 0xFF);
	outb(0x40,(count & 0xFF00) >> 8);
	return;
}