#include <kernel/kmalloc.h>
#include <stdlib.h>

void* malloc(size_t size) { return kmalloc(size); }