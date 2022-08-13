#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <multiboot.h>
#include <kernel/tty.h>
#include <kernel/serial.h>
#include <kernel/keyboard.h>
#include <kernel/descriptors_tables.h>
#include <kernel/irq_handlers.h>
#include <kernel/paging.h>
#include <kernel/pic.h>
#include <kernel/pit.h>
#include <kernel/tty.h>

#define SYSTEM_TICKS_PER_SEC 100

void kernel_main(void) {
	terminal_initialize();
	init_serial();
	write_serial("LOG START\n");
	enable_cursor(1, 2);
	update_cursor(2, 4);
	descriptor_tables_initialize();
	pit_init(SYSTEM_TICKS_PER_SEC);
    printf("i8253 (PIT) initialized @%d hz\n", SYSTEM_TICKS_PER_SEC);
	write_serialf("i8253 (PIT) initialized @%d hz\n", SYSTEM_TICKS_PER_SEC);
    pic_init();
    printf("i8259 (PIC) initialized\n");
	write_serialf("i8259 (PIC) initialized\n");
    irq_register_handler(0, sys_tick_handler);
    printf("IRQ handler set: sys_tick_handler\n");
	write_serialf("IRQ handler set: sys_tick_handler\n");
    irq_register_handler(1, sys_key_handler);
    printf("IRQ handler set: sys_key_handler\n");
	write_serialf("IRQ handler set: sys_key_handler\n");
	int i = 1233;
	printf("Welcome to kernel %i\n", i);
	printf("This kernel is made using the osdev wiki\n");
	printf(">\n");
	char output[10];
	uint8_t scancode;
	uint8_t old_scancode = 0x00;
	uint8_t escape_key = 0x01;
	int scancodeNb = 0;
	printf("test div 0 : %d\n", 0/0);
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
