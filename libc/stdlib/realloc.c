#include <kernel/kmalloc.h>
#include <stdlib.h>

void* realloc(void* ptr, size_t size) { return krealloc(ptr, size); }