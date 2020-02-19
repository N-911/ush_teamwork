#include "ush.h"

static void exit_ush() {
    printf("exit\n");
    exit(EXIT_SUCCESS);
}

static void reverse_backscape(int *position, char *line) {
    for (int i = *position; i < mx_strlen(line); i++) {
        line[i] = line[i + 1];
    }
}

void mx_exec_signal(int keycode, char *line, int *position) {
    if (keycode == CTRL_C) {
        for (int i = 0; i < mx_strlen(line); i++) {
            line[i] = '\0';
        }
    }
    if (keycode == CTRL_D)
        if (strcmp(line, "") == 0)
            exit_ush();
        else
            reverse_backscape(position,  line);
    else if (keycode == TAB) {
        //to do
    }
}

