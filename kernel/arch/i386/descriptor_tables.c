// I looked at this to implement it https://github.com/alpn/x86_starterkit
#include <kernel/descriptors_tables.h>
#include <kernel/io.h>
#include <kernel/irq_handlers.h>
#include <kernel/misc.h>
#include <kernel/serial.h>
#include <kernel/tss.h>
#include <kernel/x86.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

extern void flush_gdt();
extern void flush_idt(uint32_t);
extern void idt_initialize();
extern void flush_tss(void);

static void gdt_initialize();
void set_gdt_entry(int32_t index, uint32_t base, uint32_t limit, uint8_t access,
                   uint8_t granularity);

struct gdt_entry gdt[6];
struct gdt_ptr gdt_p;

void descriptor_tables_initialize() {
  gdt_initialize();
  idt_initialize();
}

static void gdt_initialize() {
  gdt_p.limit = (sizeof(struct gdt_entry) * 6) - 1;
  gdt_p.base = (unsigned int)gdt;
  set_gdt_entry(0, 0, 0, 0, 0);                // Null
  set_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code
  set_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data
  set_gdt_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // ring 3 code
  set_gdt_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // ring 3 data
  // install_tss(&gdt[5]);
  write_tss(5, 0x10, 0x0);
  flush_gdt();
  // flush_tss();
}

// Set the value of one GDT entry.
void set_gdt_entry(int32_t index, uint32_t base, uint32_t limit, uint8_t access,
                   uint8_t granularity) {
  gdt[index].base_low = (base & 0xFFFF);
  gdt[index].base_middle = (base >> 16) & 0xFF;
  gdt[index].base_high = (base >> 24) & 0xFF;
  gdt[index].limit_low = (limit & 0xFFFF);
  gdt[index].granularity = ((limit >> 16) & 0x0F);
  gdt[index].granularity |= (granularity & 0xF0);
  gdt[index].access = access;
}

char* exception_messages[] = {"Division By Zero",
                              "Debug",
                              "Non Maskable Interrupt",
                              "Breakpoint",
                              "Into Detected Overflow",
                              "Out of Bounds",
                              "Invalid Opcode",
                              "No Coprocessor",

                              "Double Fault",
                              "Coprocessor Segment Overrun",
                              "Bad TSS",
                              "Segment Not Present",
                              "Stack Fault",
                              "General Protection Fault",
                              "Page Fault",
                              "Unknown Interrupt",

                              "Coprocessor Fault",
                              "Alignment Check",
                              "Machine Check",
                              "Reserved",
                              "Reserved",
                              "Reserved",
                              "Reserved",
                              "Reserved",

                              "Reserved",
                              "Reserved",
                              "Reserved",
                              "Reserved",
                              "Reserved",
                              "Reserved",
                              "Reserved",
                              "Reserved"};

void handle_exception(uint32_t vector) {
  printf("Exception raised: ");
  printf(exception_messages[vector]);
  x86_halt();
}

void handle_unknown(void) { handle_exception(16); }

void x86_exception_handler(registers_t* iframe) {

  uint32_t vector = iframe->vector;

  if (31 >= vector) {
    handle_exception(vector);
  } else if (47 >= vector) {
    handle_platform_irq(iframe);
  } else {
    handle_unknown();
  }
}