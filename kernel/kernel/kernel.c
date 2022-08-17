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
#include <kernel/vfs.h>
#include <kernel/initrd.h>

#define SYSTEM_TICKS_PER_SEC 100
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

multiboot_info_t *mb_info;
multiboot_module_t *mod;

void kernel_main(multiboot_info_t* mbd) {
	mb_info = mbd;
	terminal_initialize();
	init_serial();
	write_serial("LOG START\n");
	descriptor_tables_initialize();
	pit_init(SYSTEM_TICKS_PER_SEC);
	log(LOG_ALL, true, "i8253 (PIT) initialized @%d hz\n", SYSTEM_TICKS_PER_SEC);
    pic_init();
	log(LOG_ALL, true, "i8259 (PIC) initialized\n");
    irq_register_handler(0, sys_tick_handler);
    log(LOG_ALL, true, "IRQ handler set: sys_tick_handler\n");
    irq_register_handler(1, sys_key_handler);
    log(LOG_ALL, true, "IRQ handler set: sys_key_handler\n");
	char timer_str[] = "System timer is ticking\n";
    terminal_tick_init(sizeof(timer_str));
    puts(timer_str);
    char key_str[] = "Last keypress:\n";
    terminal_keypress_init(sizeof(key_str));
    puts(key_str);
	x86_enable_int();
	/*initialise_paging();
	log(LOG_ALL, true, "Paging enabled\n");
	uint32_t *ptr = (uint32_t*)0xA0000000;
    uint32_t do_page_fault = *ptr;*/
	int i = 1233;
	printf("Welcome to kernel %i\n", i);
	printf("This kernel is made using the osdev wiki\n");
	printf("Press ESC to reboot\n");
	alert("error : %s\n", "string");
	fprintf(VFS_FD_STDOUT, "hello from fprintf stdout\n");
	fprintf(VFS_FD_SERIAL, "hello from fprintf serial\n");
	/*if (CHECK_FLAG (mb_info->flags, 2)){
    printf ("cmdline = %s\n", (char *) mb_info->cmdline);
	}*/
	if (mb_info->mods_count > 0){
		mod = (multiboot_module_t *) mb_info->mods_addr;
		write_serialf("module loaded\n");
		write_serialf("disk image: Module starts at %p and ends at %p\n", (int)mod->mod_start, (int)mod->mod_end);
		write_serialf("number of files : %d\n", initrd_get_number_files(mod->mod_start));
	}
	write_serialf("modules %d\n", mb_info->mods_count);
	write_serialf("addr %p\n", mb_info->mods_addr);
	printf("> ");
	while(1);
}
