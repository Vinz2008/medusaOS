#include <stdlib.h>
#include <string.h>

void* calloc(size_t count, size_t size){
    void* ptr = malloc(count * size);
    if (ptr != NULL){
        memset(ptr, 0, count * size);
    }
    return  ptr;
}