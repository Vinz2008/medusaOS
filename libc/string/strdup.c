#include <string.h>
#include <kernel/kmalloc.h>

char* strdup(const char* s){
    int i = (strlen(s)+1) * sizeof(char);
    char *s2 = (char*)kmalloc(i);
    if(s2 != NULL) memcpy(s2, (void*)s, i);
    return s2;
}