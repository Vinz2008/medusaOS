#include <types.h>


#define INT_START asm volatile("pusha");
#define INT_END asm volatile("popa"); \
	asm volatile("iret");


typedef struct {
	uint16_t offset_0_15;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_16_31;
} __attribute__((packed)) idt_descriptor;




extern void idt_init();
extern void exceptions_init();
extern void idt_register_interrupt(uint8_t i, uint32_t addr);
extern void add_idt_descriptor(uint8_t id, idt_descriptor desc);
