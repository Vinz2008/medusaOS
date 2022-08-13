// I looked at this to implement it https://github.com/ozkl/soso/blob/master/kernel/descriptortables.h

#include <types.h>
#include <kernel/x86.h>


void descriptor_tables_initialize();
void sys_tick_handler(x86_iframe_t* frame);

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