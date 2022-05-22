#include <types.h>
#include <stdio.h>
#include <kernel/misc.h>
#include <kernel/gdt.h>
#include <string.h>

tss_entry_t tss_entry;

struct gdt_entry gdt[6];
struct gdt_ptr gp;

static void write_tss(int32, uint16_t, uint32_t);


void encodeGdtEntry(uint8_t *target, struct gdt_entry source)
{
    if (source.limit_low > 0xFFFFF) {printf("GDT cannot encode limits larger than 0xFFFFF");}
    target[0] = source.limit_low & 0xFF;
    target[1] = (source.limit_low >> 8) & 0xFF;
    target[6] = (source.limit_low >> 16) & 0x0F;
    target[2] = source.base_low & 0xFF;
    target[3] = (source.base_low >> 8) & 0xFF;
    target[4] = (source.base_low >> 16) & 0xFF;
    target[7] = (source.base_low >> 24) & 0xFF;
    target[5] = source.access;
    target[6] |= (source.flags << 4);
}


void create_descriptor(uint32_t base, uint32_t limit, uint16_t flag){
    int64 descriptor;
 
    // Create the high 32 bit segment
    descriptor  =  limit       & 0x000F0000;         // set limit bits 19:16
    descriptor |= (flag <<  8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
    descriptor |=  base        & 0xFF000000;         // set base bits 31:24
 
    // Shift by 32 to allow for low part of segment
    descriptor <<= 32;
 
    // Create the low 32 bit segment
    descriptor |= base  << 16;                       // set base bits 15:0
    descriptor |= limit  & 0x0000FFFF;               // set limit bits 15:0
 
    printf("0x%.16llX\n", descriptor);
}


void gdt_set_gate(int num, int32 base, int32 limit, int32 access,int8 gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0X0F;
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install() {
    gp.limit = (sizeof(struct gdt_entry) * 6) - 1;
    gp.base = (unsigned int)&gdt;
    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    write_tss(5, 0x10, 0x0);
    gdt_flush();
	tss_flush();
}

static void write_tss(int32 num, uint16_t ss0, uint32_t esp0) {
    uint32_t base = (uint32_t)&tss_entry;
    uint32_t limit = base + sizeof(tss_entry);
    gdt_set_gate(num, base, limit, 0xE9, 0x00);
    memset(&tss_entry, 0x0, sizeof(tss_entry));
    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;
    tss_entry.cs = 0x0b;
    tss_entry.ss     =
		tss_entry.ds =
		tss_entry.es =
		tss_entry.fs =
		tss_entry.gs = 0x13;
	tss_entry.iomap_base = sizeof(tss_entry);
}

void set_kernel_stack(uint32_t stack) {
	tss_entry.esp0 = stack;
}


