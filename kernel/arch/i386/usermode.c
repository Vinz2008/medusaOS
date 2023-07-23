#include <kernel/tss.h>
#include <kernel/elf.h>

#define PUSH(stack, type, item) stack -= sizeof(type); \
							*((type *) stack) = item



void enter_user_jmp(uintptr_t location, int argc, char ** argv, uintptr_t stack) {
    // disable interrupts
    set_kernel_stack()
    PUSH(stack, uintptr_t, (uintptr_t)argv);
	PUSH(stack, int, argc);
    asm volatile(
			"mov %3, %%esp\n"
			"pushl $0xDECADE21\n"  /* Magic */
			"mov $0x23, %%ax\n"    /* Segment selector */
			"mov %%ax, %%ds\n"
			"mov %%ax, %%es\n"
			"mov %%ax, %%fs\n"
			"mov %%ax, %%gs\n"
			"mov %%esp, %%eax\n"   /* Stack -> EAX */
			"pushl $0x23\n"        /* Segment selector again */
			"pushl %%eax\n"
			"pushf\n"              /* Push flags */
			"popl %%eax\n"         /* Fix the Interrupt flag */
			"orl  $0x200, %%eax\n"
			"pushl %%eax\n"
			"pushl $0x1B\n"
			"pushl %0\n"           /* Push the entry point */
			"iret\n"
			: : "m"(location), "r"(argc), "r"(argv), "r"(stack) : "%ax", "%esp", "%eax");
}