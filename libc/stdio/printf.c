#include <kernel/tty.h>
#include <kernel/vfs.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {
  const unsigned char* bytes = (const unsigned char*)data;
  for (size_t i = 0; i < length; i++) {
    if (putchar(bytes[i]) == EOF) {
      return false;
    }
  }
  return true;
}

int printf(const char* restrict format, ...) {
  va_list parameters;
  va_start(parameters, format);
  return vprintf(format, parameters);
}

int vprintf(const char* restrict format, va_list parameters) {
  return vfprintf(VFS_FD_STDOUT, format, parameters);
}
