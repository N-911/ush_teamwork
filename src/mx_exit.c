#include "ush.h"

int mx_exit(t_shell *m_s, t_process *p) {
    int exit_code = m_s->exit_code;
    int flag = 0;
    int start = 0;

    if(p->argv[1] != NULL && p->argv[2] != NULL) {
        mx_printerr("ush: exit: too many arguments\n");
        return 1;
    }
    if (m_s->jobs_stack->top && m_s->exit_flag == 0) {
        mx_printerr("ush: you have suspended jobs.\n");
        m_s->exit_flag = 1;
        return 1;
    }
    if (p->argv[1] != NULL) {
        if (p->argv[1][0] == '+' || p->argv[1][0] == '-') {
            if (p->argv[1][1] == '\0')
                flag++;
            start++;
        }
        for (int i = start; i < mx_strlen(p->argv[1]); i++) {
            if(!mx_isdigit(p->argv[1][i])) {
                flag++;
                break;
            }
        }
        if (!flag) {
            exit_code = atoi(p->argv[1]);
        }
        else {
            mx_printerr("ush: exit: ");
            mx_printerr(p->argv[1]);
            mx_printerr(": numeric argument required\n");
            exit_code = 255;
        }
    }
    mx_clear_all(m_s);
    system("leaks -q ush");
    exit(exit_code);
}
