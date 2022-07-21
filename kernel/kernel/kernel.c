#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <multiboot.h>
#include <kernel/tty.h>
#include <kernel/serial.h>
#include <kernel/keyboard.h>
#include <kernel/descriptors_tables.h>
#include <kernel/paging.h>
#include <kernel/pit.h>
#include <kernel/tty.h>

void kernel_main(void) {
	//unsigned int ebx;
	terminal_initialize();
	init_serial();
	write_serial("LOG START\n");
	enable_cursor(1, 2);
	update_cursor(2, 4);
	//printf("serial: %c",read_serial(SERIAL_COM1_BASE));
	descriptor_tables_initialize();
	int i = 1233;
	printf("Welcome to kernel %i\n", i);
	printf("This kernel is made using the osdev wiki\n");
	printf(">\n");
	char output[10];
	uint8_t scancode;
	uint8_t old_scancode = 0x00;
	uint8_t escape_key = 0x01;
	int scancodeNb = 0;
	//multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
    //unsigned int address_of_module = mbinfo->mods_addr;
	while (1)
	{
		scancode = read_scan_code();
		if (scancode == old_scancode) {
			scancode = old_scancode;
		}

		else /*if (scancode == escape_key)*/ {
			//printf("escape\n");
			hex_to_ascii(scancode, output);
			//printf("%c\n",output);
		}
		if (scancodeNb == 0x5A){
		printf("enter pressed\n");
		scancodeNb++;
		}
		old_scancode = scancode;
		//printf("%c\n", scancodes);
	}
	
}
