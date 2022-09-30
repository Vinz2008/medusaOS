#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

#ifdef __cplusplus
}
#endif

// Uninmplemented declarations for hosted cross compiler
void free(void* ptr);
void* malloc(size_t size)

#endif
