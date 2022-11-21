#include <stdlib.h>
#include <kernel/kmalloc.h>

void *realloc(void *ptr, size_t size){
    return krealloc(ptr, size);
}