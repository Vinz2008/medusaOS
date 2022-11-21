#include <stdlib.h>
#include <kernel/kmalloc.h>

void* malloc(size_t size){
    return kmalloc(size);
}