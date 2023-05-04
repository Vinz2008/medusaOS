#ifndef _STDIO_H
#define _STDIO_H 1

#include <stdarg.h>
#include <sys/cdefs.h>
#include <types.h>
#include <kernel/vfs.h>

#define LOG_SCREEN 0
#define LOG_SERIAL 1
#define LOG_STDERR 2
#define LOG_ALL 3

#define EOF (-1)
#define	F_OK	0
#define	R_OK	4
#define	W_OK	2
#define	X_OK	1

#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2
#define BUFSIZ 512
#define TMP_MAX 32


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


void log(int log_level, bool ok_printing, const char* restrict format, ...);
void alert(const char* restrict format, ...);
void green(const char* restrict format, ...);
void ok_printing_boot(const char* restrict format, ...);
void vok_printing_boot(const char* restrict format, va_list parameters);

int sprintf(char *str, const char *format, ...);

// Uninmplemented declarations for hosted cross compiler

//typedef file_descriptor_t FILE;
//typedef int FILE;
typedef struct {
    int fd;
    char* name;
} FILE;


int fclose(FILE *stream);

FILE* fopen(const char *filename, const char *mode);

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

int fseek(FILE *stream, long int offset, int whence);

long int ftell(FILE *stream);

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

void setbuf(FILE *stream, char *buffer);

extern FILE* stderr;


int fflush(FILE *stream);

#ifdef __cplusplus
}
#endif

#endif
