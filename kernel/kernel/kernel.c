#include <stdio.h>
#include <stdint.h>
#include <kernel/tty.h>
#include <stddef.h>

void kernel_main(void) {
	terminal_initialize();
	printf("Welcome to the OS\n");
}
