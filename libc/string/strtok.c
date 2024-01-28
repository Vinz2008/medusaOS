#include <stddef.h>
#include <string.h>

char* strtok(char* str, const char* delimiters) {
  /* On the first call, we expect a string as an argument, then each subsequent
     call starts right after the last token and we expect str to be a NULL
     pointer */
  static char* next_start;
  if (str != NULL)
    next_start = str;
  if (!*next_start)
    return NULL;

  /* The token only begins at the first non-delimiter character */
  while (*next_start && strchr(delimiters, *next_start) != NULL)
    ++next_start;
  if (!*next_start)
    return NULL;

  /* The token ends at the first delimiter character */
  size_t length = 0;
  while (*next_start && !strchr(delimiters, *next_start)) {
    ++next_start;
    ++length;
  }
  if (!*next_start)
    return NULL;

  /* Keep in memory the position of the token for other subsequent call, and
     return the beginning of the token found */
  char* token = next_start;
  ++next_start;
  *token = '\0';
  return token - length;
}

// from https://github.com/perryizgr8/strtok_r/blob/master/strtok_r.c
char* strtok_r(char* text, const char delimitor, char** save_this) {
  if (save_this == NULL) {
    return NULL;
  }
  if (text != NULL) {
    /* New text. */
    int i = 0;
    while (text[i] != '\0') {
      if (text[i] == delimitor) {
        text[i] = '\0';
        *save_this = &text[i + 1];
        return text;
      }
      i++;
    }
  } else if ((save_this != NULL) && (*save_this != NULL)) {
    /* Old text. */
    int i = 0;
    char* start = *save_this;
    while ((*save_this)[i] != '\0') {
      if ((*save_this)[i] == delimitor) {
        (*save_this)[i] = '\0';
        *save_this = &((*save_this)[i + 1]);
        return start;
      }
      i++;
    }
    *save_this = NULL;
    save_this = NULL;
    return start;
  }
  return NULL;
}