#include <stdint.h>
#include <sys/types.h>
#include <types.h>

// Uninmplemented declarations for hosted cross compiler

typedef int32_t intptr_t;
typedef signed long long int int64_t;

pid_t fork();
int execv(const char* path, char* const argv[]);
int execvp(const char* file, char* const argv[]);
int execve(const char* filename, char* const argv[], char* const envp[]);
pid_t getpid();
