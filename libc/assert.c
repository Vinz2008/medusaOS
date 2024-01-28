#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void __assert_fail(const char* assertion, int expression, const char* file,
                   int line) {
  if (expression == false) {
    log(LOG_SERIAL, false, "%s at %d, assertion %s failed\n", file, line,
        assertion);
    abort();
  }
}