#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void*, const void*, size_t);
void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char*);
void int_to_ascii_base(int n, char str[], int base);
void int_to_ascii(int n, char str[]);
void append(char s[], char n);
void hex_to_ascii(int n, char str[]);
void reverse(char* str1);
char* strcat(char* dest, const char* src);
bool startswith(char str[], char code[]);
char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t num);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
char* strdup(const char* s);
char* strstr(const char* str1, const char* str2);
char* strchr(const char* str, int c);
char* strtok(char* str, const char* delimiters);
char* strtok_r(char* text, const char delimitor, char** save_this);

#ifdef __cplusplus
}
#endif

#endif
