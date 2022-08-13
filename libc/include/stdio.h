#ifndef _STDIO_H
#define _STDIO_H 1

#include <stdarg.h>
#include <sys/cdefs.h>
#include <types.h>
#include <kernel/vfs.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict, ...);
int vprintf(const char* __restrict, va_list parameters);
int putchar(int);
int puts(const char*);

int fprintf(file_descriptor_t file, const char* format, ...);
int fputs(const char* string, file_descriptor_t file);
int vfprintf(file_descriptor_t file, const char* format, va_list parameters);
int fputchar(char c, file_descriptor_t file);

#ifdef __cplusplus
}
#endif

#endif
