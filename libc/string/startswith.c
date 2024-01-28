#include <stdbool.h>
#include <string.h>

bool startswith(char str[], char line[]) {
  int similarity = 0;
  int length = strlen(str);
  int i;
  for (i = 0; i < length /*-1*/; i++) {
    if (str[i] == line[i]) {
      similarity++;
    }
  }
  if (similarity >= length) {
    return true;
  } else {
    return false;
  }
}