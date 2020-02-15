#include "ush.h"

int mx_launch_builtin(t_shell *m_s, t_process *p, int job_id) {
    int (*builtin_functions[])(t_shell *m_s, t_process *p) = {&mx_env, &mx_export, &mx_unset,
        &mx_echo, &mx_jobs, &mx_fg, &mx_bg, &mx_cd, &mx_pwd, &mx_which, &mx_exit, NULL};
    int status = 0;
    int shell_is_interactive = isatty(STDIN_FILENO);
    pid_t child_pid;

    p->status = STATUS_RUNNING;
    if (p->type == 4 || p->type == 5 || p->type == 6) {
        if(!p->pipe && p->foreground && m_s->jobs[job_id]->first_process->next == NULL)
            mx_remove_job(m_s, job_id);
    }
    // if pipe or in foreground -> fork
    if (p->pipe || !p->foreground) {
        child_pid = fork();
        p->pid = child_pid;
        if (child_pid < 0) {
            perror("error fork");
            exit(1);
        }
        else if (child_pid == 0) {
            if (shell_is_interactive) {
                if (m_s->jobs[job_id]->pgid == 0)
                    m_s->jobs[job_id]->pgid = child_pid;
                setpgid(child_pid, m_s->jobs[job_id]->pgid);
                if (m_s->jobs[job_id]->foreground)
                    tcsetpgrp(STDIN_FILENO, m_s->jobs[job_id]->pgid);
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                signal(SIGTTIN, SIG_DFL);
                signal(SIGTTOU, SIG_DFL);
                signal(SIGPIPE, mx_sig_h);
            }
//            //****************
//            mx_print_color(MAG, "child\t");
//            mx_print_color(MAG, "child pid\t");
//            mx_print_color(MAG, mx_itoa(p->pid));
//            mx_print_color(MAG, "\tm_s->jobs[job_id]->pgid ");
//            mx_printstr("\n");
//            //****************

            if (p->infile != STDIN_FILENO) {
                dup2(p->infile, STDIN_FILENO);
                close(p->infile);
            }
            if (p->outfile != STDOUT_FILENO) {
                dup2(p->outfile, STDOUT_FILENO);
                close(p->outfile);
            }
            if (p->errfile != STDERR_FILENO) {
                dup2(p->errfile, STDERR_FILENO);
                close(p->errfile);
            }
            status = builtin_functions[p->type](m_s, p);
            p->status = STATUS_DONE;
            exit (status);
        }
        else {
            if (shell_is_interactive) {
               // pid_t pid = child_pid;
                if (m_s->jobs[job_id]->pgid == 0)
                    m_s->jobs[job_id]->pgid = child_pid;
                setpgid (child_pid, m_s->jobs[job_id]->pgid);
            }
        }
    }
    else {
        int defoult;
        if(p->output_path) {
            defoult = dup(1);
            if (p->outfile != STDOUT_FILENO) {
                lseek(p->outfile, 0, SEEK_END);  
                dup2(p->outfile, STDOUT_FILENO);
                close(p->outfile);
            }
            if (p->infile != STDIN_FILENO) {  
                dup2(p->infile, STDIN_FILENO);
                close(p->infile);
            }
        }
        status = builtin_functions[p->type](m_s, p);
        p->status = STATUS_DONE;
        if(p->output_path) {
            if (p->outfile != STDOUT_FILENO) {
                dup2(defoult, 1);
                close(defoult);
            }
        }
    }
    p->status = STATUS_DONE;
    return status;
}
