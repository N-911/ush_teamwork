#include "ush.h"

void mx_printerr_red(char *c) {
    mx_printerr(RED);
    mx_printerr(c);
    mx_printerr(RESET);
}

void mx_print_color(char *macros, char *str) {
    mx_printstr(macros);
    mx_printstr(str);
    mx_printstr(RESET);
}

int mx_get_char_index_reverse(const char *str, char c) {
    char *s = (char *)str;
    int i;

    if (!str || !*str)
        return -2;
    i = mx_strlen(s) - 1;
    while (s[i]) {
        if (s[i] == c)
            return i;
        i--;
    }
    return -1;
}

bool mx_isdelim (char c, char *delim) {
    if (!c || !delim)
        return false;
    for (int i = 0; delim[i]; i++) {
        if (delim[i] == c)
            return true;
    }
    return false;
}

bool mx_check_allocation_error(const void *c) {
    if (c == NULL) {
        mx_printerr_red("allocation error\n");
        return true;
    }
    return false;
}
