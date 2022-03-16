#include <kernel/gdt.h>
#include <types.h>
#include <stdio.h>


struct gdt {
    unsigned int address;
    unsigned short size;
} __attribute__((packed));
