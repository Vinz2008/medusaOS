// I looked at this to implement it https://github.com/ozkl/soso/blob/master/kernel/descriptortables.c
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/misc.h>
#include <kernel/io.h>
#include <kernel/idt.h>
#include <kernel/task.h>
#include <kernel/descriptors_tables.h>
#include <kernel/serial.h>

extern void flush_gdt(uint32_t);
extern void flush_idt(uint32_t);
extern void flush_tss();

GdtEntry g_gdt_entries[6];
GdtPointer g_gdt_pointer;
IdtEntry g_idt_entries[256];
IdtPointer g_idt_pointer;
Tss g_tss;



static uint32_t idt_location = 0;
static uint32_t idtr_location = 0;
static uint16_t idt_size = 0x800;
static uint8_t __idt_setup = 0;
static uint8_t test_success = 0;
static uint32_t test_timeout = 0x1000;

void __idt_default_handler();
void __idt_test_handler();
extern void _set_idtr();

static void gdt_initialize();
static void idt_initialize();

static void set_gdt_entry(int32 num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

void descriptor_tables_initialize()
{
    gdt_initialize();
    printf("gdt initialized\n");
    write_serial("gdt initialized\n");
    idt_initialize();

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


void irq_timer();

static void idt_initialize(){
    idt_location = 0x2000;
    write_serialf("Location: 0x%x\n", idt_location);
    write_serialf("IDTR location: 0x%x\n", idtr_location);
    idtr_location = 0x10F0;
    __idt_setup = 1;
    for(uint8_t i = 0; i < 255; i++){
		idt_register_interrupt(i, (uint32_t)&__idt_default_handler);
	}
    idt_register_interrupt(0x2f, (uint32_t)&__idt_test_handler);
	idt_register_interrupt(0x2e, (uint32_t)&schedule);
    write_serialf("Registered all interrupts to default handler.\n");
    *(uint16_t*)idtr_location = idt_size - 1;
	*(uint32_t*)(idtr_location + 2) = idt_location;
    write_serialf("IDTR.size = 0x%x IDTR.offset = 0x%x\n", *(uint16_t*)idtr_location, *(uint32_t*)(idtr_location + 2));
    _set_idtr();
    write_serialf("idtr set\n");
    /*asm volatile("int $0x2f");
    write_serialf("int $0x2f\n");
	while(test_timeout-- != 0)
	{
		if(test_success != 0)
		{
			write_serialf("Test succeeded, disabling INT#0x2F\n");
			idt_register_interrupt(0x2F, (uint32_t)&__idt_default_handler);
			break;
		}
	}
	if(!test_success)
		write_serialf("IDT link is offline (timeout).");
        abort();*/
	return;
}

void __idt_test_handler()
{
	INT_START;
	test_success = 1;
	INT_END;
}

void idt_register_interrupt(uint8_t i, uint32_t callback)
{
	if(!__idt_setup){
        write_serialf("Invalid IDT!\n");
        abort();
    }
	*(uint16_t*)(idt_location + 8*i + 0) = (uint16_t)(callback & 0x0000ffff);
	*(uint16_t*)(idt_location + 8*i + 2) = (uint16_t)0x8;
	*(uint8_t*) (idt_location + 8*i + 4) = 0x00;
	*(uint8_t*) (idt_location + 8*i + 5) = 0x8e;//0 | IDT_32BIT_INTERRUPT_GATE | IDT_PRESENT;
	*(uint16_t*)(idt_location + 8*i + 6) = (uint16_t)((callback & 0xffff0000) >> 16);
	if(test_success) write_serialf("registered INT %i\n", i);
	return;
}