#include "ush.h"

int mx_launch_process(t_shell *m_s, t_process *p, pid_t pgid, int infile, int outfile, int errfile) {

//    pid_t pgid = m_s->jobs[job_id]->pgid;
    p->status = STATUS_RUNNING;

    int shell_is_interactive = isatty(STDIN_FILENO);  //!!
    m_s->history_index = 0;
    if (shell_is_interactive) {
        p->pid = getpid();
        if (pgid == 0)
            pgid = p->pid;
        setpgid(p->pid, pgid);   //Процесс может присоединиться к группе или создать новую группу процессов
        if (p->foreground)
            tcsetpgrp(STDIN_FILENO, pgid);
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
    }
    p->pid = getpid();
/*
    if (pgid > 0) {
        setpgid(0, pgid);
    } else {
        pgid = p->pid;
        setpgid(0, pgid);
    }
*/
    if (infile != STDIN_FILENO) {
        dup2(infile, STDIN_FILENO);
            close(infile);
    }
    if (outfile != STDOUT_FILENO) {
        dup2(outfile, STDOUT_FILENO);
        close(outfile);
    }
    if (errfile != STDERR_FILENO) {
        dup2(errfile, STDERR_FILENO);
        close(errfile);
    }
    if (execvp(p->argv[0], p->argv) < 0) {
        mx_printerr("u$h: command not found: ");
        mx_printerr(p->argv[0]);
        mx_printerr("\n");
        // perror("execvp");
        _exit(EXIT_FAILURE);
    }
    _exit(0);
    return (p->exit_code);
}
