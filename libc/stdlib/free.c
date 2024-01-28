#include <kernel/kmalloc.h>
#include <stdlib.h>

void free(void* ptr) { kfree(ptr); }