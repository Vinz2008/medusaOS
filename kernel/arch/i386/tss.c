#include <kernel/tss.h>
#include <types.h>
#include <string.h>
#include <kernel/descriptors_tables.h>


tss_entry_t tss_entry;

void write_tss(struct gdt_entry* g){
    uint32_t base = (uint32_t)&tss_entry;
    uint32_t limit = sizeof(tss_entry);
    g->limit_low = limit;
    g->base_low = base;
    g->access = 1;
    g->base_middle = (base >> 16) & 0xFF;
    g->granularity = 0;
    g->base_high = (base >> 24) & 0xFF;
    memset(&tss_entry, 0, sizeof(tss_entry));
    //tss_entry.ss0  = REPLACE_KERNEL_DATA_SEGMENT;
    //tss_entry.esp0 = REPLACE_KERNEL_STACK_ADDRESS;
}

void set_kernel_stack(uint32_t stack) { // Used when an interrupt occurs
	tss_entry.esp0 = stack;
}