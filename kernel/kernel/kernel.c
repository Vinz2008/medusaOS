#include <stdio.h>
#include <stdint.h>
#include <kernel/tty.h>
#include <stddef.h>

void kernel_main(void) {
	terminal_initialize();
	int i = 1;
	printf("Welcome to the OS %i\n", i);
	printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
}
