#include <stdio.h>
#include <stdint.h>
#include <kernel/tty.h>
#include <kernel/keyboard.h>
#include <kernel/gdt.h>
#include <stddef.h>
#include <string.h>

void kernel_main(void) {
	terminal_initialize();
	//gdt_install();
	int i = 1233;
	printf("Welcome to kernel %i\n", i);
	printf("This kernel is made using the osdev wiki\n");
	char output[10];
	uint8_t scancode;
	uint8_t old_scancode = 0x00;
	uint8_t escape_key = 0x01;
	while (1)
	{
		scancode = read_scan_code();
		if (scancode == old_scancode) {
			scancode = old_scancode;
		}

		else /*if (scancode == escape_key)*/ {
			//printf("escape\n");
			hex_to_ascii(scancode, output);
			printf("%c\n",output);
		}
		old_scancode = scancode;
		//printf("%c\n", scancodes);
	}
	
}
