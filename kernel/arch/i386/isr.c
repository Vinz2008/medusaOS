#include <kernel/isr.h>
#include <stdio.h>

void isr_handler(registers_t regs)
{
   printf("received interrupt: %i\n", regs.int_no);
} 

void exception_handler() {
    __asm__ volatile ("cli; hlt");
}