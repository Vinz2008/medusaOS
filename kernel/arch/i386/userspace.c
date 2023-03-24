#include <kernel/descriptors_tables.h>

extern struct gdt_entry gdt[6];

void jump_to_userspace(){
    struct gdt_entry* ring;
}