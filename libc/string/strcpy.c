#include <stdio.h>

char* strcpy(char* dst, const char* src){
    char* s = dst;
    if(src && dst && src != dst) {
        while(*src) {*dst++=*src++;} *dst=0;
    }
    return s;
}