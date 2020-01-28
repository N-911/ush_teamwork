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
