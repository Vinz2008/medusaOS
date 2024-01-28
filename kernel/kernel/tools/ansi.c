#include <ctype.h>
#include <kernel/ansi.h>
#include <kernel/graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>
// #include <kernel/gui.h>
#include <kernel/config.h>
#include <kernel/tty_framebuffer.h>

static char nb_buf[100];

uint32_t ansi_color_to_hex(int ansi_col) {
  switch (ansi_col) {
  default:
    return -1;
  case ANSI_WHITE:
    return COLOR_WHITE;
  case ANSI_RED:
    return COLOR_RED;
  case ANSI_BLUE:
    return COLOR_BLUE;
  case ANSI_GREEN:
    return COLOR_GREEN;
  }
}

int ansi_get_number(char* sequence, int* pos) {
  memset(nb_buf, 0, 100);
  while (isalnum(sequence[*pos])) {
    append(nb_buf, sequence[*pos]);
    (*pos)++;
  }
  return atoi(nb_buf);
}

void handle_ansi_sequence(char* sequence) {
  int pos = 0;
  int mode_number = -1;
  int color_number = -1;
  if (sequence[pos] == '[') {
    pos++;
  }
  // char c = '\x1B';
  //  find mode number
  if (isalnum(sequence[pos])) {
    mode_number = ansi_get_number(sequence, &pos);
    log(LOG_SERIAL, false, "mode_number : %d\n", mode_number);
  }

  if (sequence[pos] == ';') {
    pos++;
    if (isalnum(sequence[pos])) {
      color_number = ansi_get_number(sequence, &pos);
      log(LOG_SERIAL, false, "color_number : %d\n", color_number);
    }
  }
  if (sequence[pos] == 'm') {
    // set mode with mode number
    if (mode_number == 0) {
      terminal_framebuffer_reset_color();
    }
    pos++;
  }
  // set color with color number
#if GUI_MODE
#else
  terminal_framebuffer_setcolor(ansi_color_to_hex(color_number));
#endif
}