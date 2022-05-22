#include <types.h>
#include <kernel/idt.h>


/*void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_desc_t* descriptor = &idt[vector];
    descriptor->base_low       = (int64)isr & 0xFFFF;
    descriptor->cs             = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->base_mid       = ((int64)isr >> 16) & 0xFFFF;
    descriptor->base_high      = ((int64)isr >> 32) & 0xFFFFFFFF;
    descriptor->rsv0           = 0;
}
*/

extern void* isr_stub_table[];


void idt_init() {
    /*idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;
 
    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }
 
    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("sti");*/
}