#include <kernel/serial.h>
#include <stdbool.h>
#include <stdio.h>

void log(int log_level, bool ok_printing, const char* restrict format, ...) {
  va_list parameters;
  va_start(parameters, format);
  switch (log_level) {
  case LOG_SCREEN:
    if (ok_printing) {
      ok_printing_boot(format, parameters);
    } else {
      vprintf(format, parameters);
    }
    break;
  case LOG_SERIAL:
    vwrite_serialf(format, parameters);
    break;
  case LOG_ALL:
    if (ok_printing) {
      ok_printing_boot(format, parameters);
    } else {
      vprintf(format, parameters);
    }
    vwrite_serialf(format, parameters);
    break;
  default:
    alert("unknown log level : %i\n", log_level);
    break;
  }
}