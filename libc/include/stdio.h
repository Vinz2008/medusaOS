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
int putchar(int);
int puts(const char*);

int fprintf(file_descriptor_t file, const char* format, ...);
int vfprintf(file_descriptor_t file, const char* format, va_list args);
int fputchar(char c, file_descriptor_t file);

#ifdef __cplusplus
}
#endif

#endif
