#include "ush.h"

static void exit_ush(t_shell *m_s) {
    printf("exit\n");
    mx_clear_all(m_s);
    exit(EXIT_SUCCESS);
}

static void reverse_backscape(int *position, char *line) {
    for (int i = *position; i < mx_strlen(line); i++) {
        line[i] = line[i + 1];
    }
}

void mx_exec_signal(int keycode, char **line, int *position, t_shell *m_s) {
    if (keycode == MX_CTRL_C) {
        for (int i = 0; i < mx_strlen(*line); i++) {
            *line[i] = '\0';
        }
    }
    if (keycode == MX_CTRL_D)
        if (strcmp(*line, "") == 0)
            exit_ush(m_s);
        else
            reverse_backscape(position,  *line);
    else if (keycode == MX_TAB) {
        // To do
    }
}
