#include <stdio.h>
#include <string.h>
#include <types.h>

void hex_to_ascii(int n, char str[]) {
  memset(str, 0, strlen(str) * sizeof(char));
  append(str, '0');
  append(str, 'x');
  char zeros = 0;

  uint32_t tmp;
  int i;
  for (i = 28; i > 0; i -= 4) {
    tmp = (n >> i) & 0xF;
    if (tmp == 0 && zeros == 0) {
      continue;
    }
    zeros = 1;
    if (tmp > 0xA) {
      append(str, tmp - 0xA + 'a');
    } else {
      append(str, tmp + '0');
    }
  }

  tmp = n & 0xF;
  if (tmp >= 0xA) {
    append(str, tmp - 0xA + 'a');
  } else {
    append(str, tmp + '0');
  }
}
