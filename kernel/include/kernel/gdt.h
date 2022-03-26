#include <types.h>

 struct gdt {
        unsigned int address;
        unsigned short size;
    } __attribute__((packed));

struct gdt_entry {
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_middle;
	unsigned char access;
	unsigned char granularity;
	unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr {
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));


extern void gdt_flush();

void gdt_install();

void gdt_set_gate(int num, int32 base, int32 limit, int32 access,int8 gran);

void set_kernel_stack(uint32_t stack);



