#include "ush.h"

int mx_true(t_shell *m_s, t_process *p) {
    m_s->exit_flag = 0;
    p->exit_code = 0;
    return 0;
}
