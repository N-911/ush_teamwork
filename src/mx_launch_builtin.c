#include "ush.h"

int mx_launch_builtin(t_shell *m_s, t_process *p) {
    int (*builtin_functions[])(t_shell *m_s, t_process *p) = {&mx_env, &mx_export, &mx_unset,
        &mx_echo, &mx_jobs, &mx_fg, &mx_bg, &mx_cd, &mx_pwd, &mx_which, &mx_exit, NULL};
    int status = 0;

    if (p->pipe) {
        pid_t pid;
        int status;

        pid = fork();
        if (pid == 0) {
            if (p->outfile != STDOUT_FILENO) {
                dup2(p->outfile, STDOUT_FILENO);
                close(p->outfile);
            }
            status = builtin_functions[p->type](m_s, p);
            exit (status);
        }
        else {
            wait(&status);
        }   
    }
    else {
        (status = builtin_functions[p->type](m_s, p));    
    }
    return status;
}
