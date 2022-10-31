#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <multiboot.h>
#include <kernel/config.h>
#include <kernel/gui.h>
#include <kernel/tty.h>
#include <kernel/serial.h>
#include <kernel/keyboard.h>
#include <kernel/descriptors_tables.h>
#include <kernel/irq_handlers.h>
#include <kernel/pic.h>
#include <kernel/pit.h>
#include <kernel/nmi.h>
#include <kernel/vfs.h>
#include <kernel/initrd.h>
#include <kernel/rtc.h>
#include <kernel/kheap.h>
#include <kernel/syscall.h>

#define SYSTEM_TICKS_PER_SEC 100
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

extern uint32_t end;
extern uint32_t placement_address;

multiboot_info_t *mb_info;
multiboot_module_t *mod;

void kernel_main(multiboot_info_t* mbd) {
	mb_info = mbd;
#if GUI_MODE
	init_gui();
#else
	terminal_initialize();
#endif
	init_serial();
	write_serial("LOG START\n");
	descriptor_tables_initialize();
	putchar(' ');
	log(LOG_ALL, true, "gdt initialized\n");
	log(LOG_ALL, true, "idt initialized\n");
	pit_init(SYSTEM_TICKS_PER_SEC);
	log(LOG_ALL, true, "i8253 (PIT) initialized @%d hz\n", SYSTEM_TICKS_PER_SEC);
    pic_init();
	log(LOG_ALL, true, "i8259 (PIC) initialized\n");
    irq_register_handler(0, sys_tick_handler);
    log(LOG_ALL, true, "IRQ handler set: sys_tick_handler\n");
    irq_register_handler(1, sys_key_handler);
    log(LOG_ALL, true, "IRQ handler set: sys_key_handler\n");
#if GUI_MODE
#else
	char timer_str[] = "System timer is ticking\n";
    terminal_tick_init(sizeof(timer_str));
	terminal_enable_tick();
    puts(timer_str);
    char key_str[] = "Last keypress:\n";
    terminal_keypress_init(sizeof(key_str));
	terminal_enable_keypress();
    puts(key_str);
#endif
	x86_enable_int();
	NMI_enable();
	log(LOG_SERIAL, false, "NMI enabled\n");
	sse_init();
	log(LOG_SERIAL, false, "SSE initialized\n");
	init_syscalls();
	log(LOG_ALL, true, "Syscalls enabled\n");
	if (mb_info->mods_count > 0){
		mod = (multiboot_module_t *) mb_info->mods_addr;
		uint32_t initrd_location = *((uint32_t*)mb_info->mods_addr);
   		uint32_t initrd_end = *(uint32_t*)(mb_info->mods_addr+4);
		placement_address = initrd_end;
		write_serialf("module loaded\n");
		write_serialf("disk image: Module starts at %p and ends at %p\n",initrd_location, initrd_end);
		//write_serialf("number of files : %d\n", initrd_get_number_files(mod->mod_start));
	}
	write_serialf("modules %d\n", mb_info->mods_count);
	unsigned int* modules = (unsigned int*)mb_info->mods_addr;
	log(LOG_SERIAL, false, "available memory from bios : %d\n",mb_info->mem_lower);
	int* test = kmalloc(sizeof(int));
	*test = 2;
	log(LOG_SERIAL, false, "pointer returned : %p\n", test);
	//paging_enable();
	/*initialise_paging();
	log(LOG_SERIAL, false, "Paging enabled\n");*/
	int* test2 = kmalloc(sizeof(int));
	int* test3 = kmalloc(sizeof(int));
	*test2 = 4;
	*test3 = 6;
	log(LOG_SERIAL, false, "pointer returned : %p\n", test2);
	log(LOG_SERIAL, false, "pointer returned : %p\n", test3);
	log(LOG_SERIAL, false, "end of the kernel : %p\n", &end);
	//log(LOG_ALL, true, "Paging enabled\n");
	//uint32_t *ptr = (uint32_t*)0xA0000000;
    //uint32_t do_page_fault = *ptr;
	int i = 1233;
#if GUI_MODE
#else
	printf("Welcome to kernel %i\n", i);
	printf("This kernel is made using the osdev wiki\n");
	printf("Press ESC to reboot\n");
	alert("error : %s\n", "string");
	fprintf(VFS_FD_STDOUT, "hello from fprintf stdout\n");
#endif
	fprintf(VFS_FD_SERIAL, "hello from fprintf serial\n");
	read_rtc_date();
	/*if (CHECK_FLAG (mb_info->flags, 2)){
    printf ("cmdline = %s\n", (char *) mb_info->cmdline);
	}*/
	//printf("\x9B1;31m\n");
#if GUI_MODE
#else
	printf("> ");
#endif
	while(1);
}
