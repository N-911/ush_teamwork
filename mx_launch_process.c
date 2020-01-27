#include "ush.h"

int mx_launch_process(t_shell *m_s, t_process *p, int  job_id, int infile, int outfile, int errfile) {
    //  int (*builtin_functions[])(t_shell *m_s, t_process *p) = {&mx_echo, &mx_jobs, &mx_fg, &mx_exit, NULL};
    int status = 0;
    pid_t child_pid;
    pid_t pgid = m_s->jobs[job_id]->pgid;
    p->status = STATUS_RUNNING;
    int shell_is_interactive = isatty(STDIN_FILENO);  //!!

    child_pid = fork();
    //TELL_WAIT();
    if (child_pid < 0) {
        perror("error fork");
        exit(1);
    }
        //child process
    else if (child_pid == 0) {
        //TELL_PARENT(getpgid(0));
        mx_printstr("child \n");
        if (shell_is_interactive) {
            p->pid = getpid();
            if (pgid > 0)
                setpgid(0, pgid);
            else {
                pgid = p->pid;
                setpgid(0, pgid);
            }
            if (p->foreground)
                tcsetpgrp(STDIN_FILENO, pgid);
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);
            signal(SIGCHLD, SIG_DFL);
        }
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
                _exit(EXIT_SUCCESS);
        }
            _exit(0);
    }
        //parrent process
    else {
        //WAIT_CHILD();
        mx_printstr("parent\n");
        p->pid = child_pid;  //PID CHILD

        if (shell_is_interactive) {
            if (!pgid)
                pgid = child_pid;
            setpgid(child_pid, pgid);
        }

        if (m_s->jobs[job_id]->foreground == FOREGROUND) {
            tcsetpgrp(0, pgid);
            status = mx_wait_job(m_s, job_id);
            signal(SIGTTOU, SIG_IGN);
            tcsetpgrp(0, getpid());
            signal(SIGTTOU, SIG_DFL);
            mx_print_job_status(m_s, job_id);
//            if (job_id > 0 && mx_job_completed(m_s, job_id)) {
//                //mx_print_job_status(m_s, job_id);
//                mx_remove_job(m_s, job_id);
  //          }
        }

    }
    return (p->exit_code);
}
