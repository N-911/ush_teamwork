#include "ush.h"

char *mx_ush_read_line(void) {
    char *line = NULL;
    size_t bufsize = 0; // have getline allocate a buffer for us

    getline(&line, &bufsize, stdin);
    return line;
}
