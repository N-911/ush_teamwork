#include "ush.h"

static int count_options(char **args) {
    int n_options = 0;

    for (int i = 1; args[i]; i++) {
        n_options++;
    }
    return n_options;
}
/*
 *  builtin set
 */
int mx_set(t_shell *m_s, t_process *p) {
    int exit_code = m_s->exit_code;
    int n_options = count_options(p->argv);

    if (n_options == 0)
        for (t_export *q = m_s->variables; q; q = q->next) {
            mx_printstr(q->name);
            mx_printstr("=");
            mx_printstr(q->value);
            mx_printstr("\n");
        }
    return exit_code;
}
