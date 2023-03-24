#include <kernel/tss.h>
#include <types.h>
#include <string.h>
#include <kernel/descriptors_tables.h>


tss_entry_t tss_entry;



void write_tss(/*struct gdt_entry* g*/ int32_t num, uint16_t ss0, uint32_t esp0){

    uint32_t base = (uint32_t)&tss_entry;
    uint32_t limit = base + sizeof(tss_entry);
    set_gdt_entry(num, base, limit, 0xE9, 0x00);
    /*g->limit_low = limit;
    g->base_low = base;
    g->access = 1;
    g->base_middle = (base >> 16) & 0xFF;
    g->granularity = 0;
    g->base_high = (base >> 24) & 0xFF;*/
    memset(&tss_entry, 0, sizeof(tss_entry));
    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;
    tss_entry.cs = 0x0b;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
    tss_entry.iomap_base = sizeof(tss_entry);
    //tss_entry.ss0  = REPLACE_KERNEL_DATA_SEGMENT;
    //tss_entry.esp0 = REPLACE_KERNEL_STACK_ADDRESS;
}

void set_kernel_stack(uint32_t stack) { // Used when an interrupt occurs
	tss_entry.esp0 = stack;
}

void install_tss(){
    tss_entry.ss0 = 0x10;
    tss_entry.iomap_base = (uint16_t) sizeof(tss_entry_t);
}
