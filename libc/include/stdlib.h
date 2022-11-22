#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <stddef.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

long strtol(const char *nptr, char **endptr, int base);

void* malloc(size_t size);
void* realloc(void *ptr, size_t size);
void* calloc(size_t count, size_t size);
int atoi(const char *str);

#ifdef __cplusplus
}
#endif

// Uninmplemented declarations for hosted cross compiler
void free(void* ptr);
int atexit(void (*func)(void));
char *getenv(const char *name);
//int abs(int x);

#endif
