#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <stddef.h>
#include <sys/cdefs.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RAND_MAX 32767
typedef struct {
  int quot;
  int rem;
} div_t;

typedef struct {
  long quot;
  long rem;
} ldiv_t;

__attribute__((__noreturn__)) void abort(void);

long strtol(const char* nptr, char** endptr, int base);

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t count, size_t size);
int atoi(const char* str);
int abs(int x);
long labs(long i);
div_t div(int num, int denom);
ldiv_t ldiv(long num, long denom);

extern uint32_t _Randseed;

int rand();

#ifdef __cplusplus
}
#endif

// Uninmplemented declarations for hosted cross compiler
void free(void* ptr);
int atexit(void (*func)(void));
char* getenv(const char* name);
void exit(int status);

#endif
