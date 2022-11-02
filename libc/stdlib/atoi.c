#include <stdlib.h>
#include <string.h>

int atoi(const char* str){
    return(strtol(str, (char **)0, 10));
}