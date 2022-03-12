#include <stdio.h>
#include <stdint.h>
#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <stddef.h>

void kernel_main(void) {
	terminal_initialize();
	int i = 1;
	printf("Welcome to kernel %i\n", i);
	printf("This kernel is made using the osdev wiki\n");
}
