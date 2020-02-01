#include "ush.h"
/*
*  read line from stdin except last char ('\n')
*/
char *mx_ush_read_line(void) {
    char *line = NULL;
    char *res = NULL;
    size_t bufsize = 0; // have getline allocate a buffer for us

    getline(&line, &bufsize, stdin);
    res = mx_strndup(line, mx_strlen(line) - 1);
    mx_strdel(&line);
    return res;
}
