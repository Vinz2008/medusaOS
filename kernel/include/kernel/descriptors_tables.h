// I looked at this to implement it https://github.com/ozkl/soso/blob/master/kernel/descriptortables.h

#ifndef _DESCRIPTOR_TABLES_HEADER_
#define _DESCRIPTOR_TABLES_HEADER_

#include <types.h>
#include <kernel/x86.h>


void descriptor_tables_initialize();
void sys_tick_handler(registers_t* frame);

struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

extern void gdt_flush();

void set_gdt_entry(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

#endif