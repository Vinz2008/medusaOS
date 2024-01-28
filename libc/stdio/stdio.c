#include <stdio.h>

// took the following from dietlibc
FILE __stderr;
FILE* stderr = &__stderr;