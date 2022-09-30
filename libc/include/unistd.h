#include <types.h>

// Uninmplemented declarations for hosted cross compiler

typedef int pid_t;
typedef int32_t intptr_t;

void fork();
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execve(const char *filename, char *const argv[],char *const envp[]); 
void getpid();
