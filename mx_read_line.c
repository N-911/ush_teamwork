#include "ush.h"
char *mx_read_line2(void) {
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "ush: allocation error\n");
    exit(EXIT_FAILURE);
  }
  while (1) {
    // Read a character
    c = getchar();
    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
        buffer[position] = '\0';
        return buffer;
    }  
    else {
      buffer[position] = c;
    }
    position++;
    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "ush: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

