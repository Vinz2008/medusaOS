#include <types.h>

uint32_t kmalloc_internal(uint32_t size, int align, uint32_t *physical);
uint32_t kmalloc_aligned(uint32_t size);  // page aligned.
uint32_t kmalloc_physical(uint32_t size, uint32_t *physical); // returns a physical address.
uint32_t kmalloc_aligned_physical(uint32_t size, uint32_t *physical); // page aligned and returns a physical address.
uint32_t kmalloc(uint32_t size); // vanilla (normal). 