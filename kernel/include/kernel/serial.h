#include <stdarg.h>

#define PORT 0x3f8 

int init_serial();
void write_serial_char(char a);
void write_serial(const char* str);
void write_serialf(const char* restrict format, ...);
void vwrite_serialf(const char* restrict format, va_list parameters);