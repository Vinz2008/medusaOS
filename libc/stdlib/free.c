#include <stdlib.h>
#include <kernel/kmalloc.h>

void free(void* ptr){
    kfree(ptr);
}