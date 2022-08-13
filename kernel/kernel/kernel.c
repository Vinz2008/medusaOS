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
	char timer_str[] = "System timer is ticking\n";
    terminal_tick_init(sizeof(timer_str));
    puts(timer_str);
    char key_str[] = "Last keypress:\n";
    terminal_keypress_init(sizeof(key_str));
    puts(key_str);
	x86_enable_int();
	int i = 1233;
	printf("Welcome to kernel %i\n", i);
	printf("This kernel is made using the osdev wiki\n");
	printf("Press ESC to reboot\n");
	printf(">\n");
	while(1);
}
