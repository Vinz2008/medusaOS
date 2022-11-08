#include <stddef.h>
#include <stdint.h>

void* aligned_alloc(size_t align, size_t size);
void* kmalloc(size_t size);
void* calloc(size_t nmemb, size_t size);
void* zalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);
void* kamalloc(uint32_t size, uint32_t align);