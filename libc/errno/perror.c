#include <errno.h>
#include <stdio.h>

int errno = 0;

void strerror(int error) {
  switch (error) {
  case EDOM:
    printf("Numerical argument out of domain\n");
    break;
  case ERANGE:
    printf("Result outside a function's range\n");
    break;
  case EILSEQ:
    printf("Illegal byte sequence\n");
    break;
  default:
    printf("error unknown\n");
    break;
  }
}

void perror(const char* str) {
  printf("%s : ", str);
  strerror(errno);
}