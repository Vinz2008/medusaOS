#include <kernel/x86.h>

#define IRQ_PIT 0
#define IRQ_PS2 1

void handle_platform_irq(x86_iframe_t* frame);
void irq_register_handler(int irq, void (*handler)(x86_iframe_t*));