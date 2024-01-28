#include <kernel/kmalloc.h>
#include <kernel/tty_framebuffer.h>
#include <stdio.h>
#include <string.h>

static int getchar_pos = 0;
static char* buf = NULL;

int getchar() {
  if (buf == NULL) {
    buf = get_line_cli();
  }
  if (getchar_pos == strlen(buf)) {
    getchar_pos = 0;
    kfree(buf);
    buf = NULL;
    return EOF;
  }
  kfree(buf);
  char c = buf[getchar_pos];
  getchar_pos++;
  return c;
}