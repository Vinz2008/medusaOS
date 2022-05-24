// I looked at this to implement it https://github.com/ozkl/soso/blob/master/kernel/descriptortables.c
#include <types.h>
#include <stdio.h>
#include <kernel/misc.h>
#include <kernel/descriptors_tables.h>
#include <kernel/tss.h>

#include <string.h>


GdtEntry g_gdt_entries[6];
GdtPointer g_gdt_pointer;
Tss g_tss;

static void set_gdt_entry(int32 num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

static void gdt_initialize();

void descriptor_tables_initialize()
{
    gdt_initialize();

    //idt_initialize();

    //memset((uint8_t*)&g_interrupt_handlers, 0, sizeof(IsrFunction)*256);

    //interrupt_register(8, handle_double_fault);
    //interrupt_register(13, handle_general_protection_fault);
}


static void gdt_initialize()
{
    g_gdt_pointer.limit = (sizeof(GdtEntry) * 7) - 1;
    g_gdt_pointer.base  = (uint32_t)&g_gdt_entries;

    set_gdt_entry(0, 0, 0, 0, 0);                // 0x00 Null segment
    set_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // 0x08 Code segment
    set_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // 0x10 Data segment
    set_gdt_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // 0x18 User mode code segment
    set_gdt_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // 0x20 User mode data segment

    //TSS
    memset((uint8_t*)&g_tss, 0, sizeof(g_tss));
    g_tss.debug_flag = 0x00;
    g_tss.io_map = 0x00;
    g_tss.esp0 = 0;//0x1FFF0;
    g_tss.ss0 = 0x10;//0x18;

    g_tss.cs   = 0x0B; //from ring 3 - 0x08 | 3 = 0x0B
    g_tss.ss = g_tss.ds = g_tss.es = g_tss.fs = g_tss.gs = 0x13;
    uint32_t tss_base = (uint32_t) &g_tss;
    uint32_t tss_limit = sizeof(g_tss);
    set_gdt_entry(5, tss_base, tss_limit, 0xE9, 0x00);

    set_gdt_entry(6, 0, 0xFFFFFFFF, 0x80, 0xCF); // Thread Local Storage pointer segment

    //flush_gdt((uint32_t)&g_gdt_pointer);
    //flush_tss();
}

// Set the value of one GDT entry.
static void set_gdt_entry(int32 num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    g_gdt_entries[num].base_low    = (base & 0xFFFF);
    g_gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    g_gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    g_gdt_entries[num].limit_low   = (limit & 0xFFFF);
    g_gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    g_gdt_entries[num].granularity |= gran & 0xF0;
    g_gdt_entries[num].access      = access;
}
